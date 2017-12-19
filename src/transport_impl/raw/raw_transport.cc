#include <iomanip>
#include <stdexcept>

#include "raw_transport.h"
#include "util/huge_alloc.h"

namespace erpc {

constexpr size_t RawTransport::kMaxDataPerPkt;

// Initialize the protection domain, queue pair, and memory registration and
// deregistration functions. RECVs will be initialized later when the hugepage
// allocator is provided.
RawTransport::RawTransport(uint8_t rpc_id, uint8_t phy_port)
    : Transport(TransportType::kInfiniBand, rpc_id, phy_port) {
  resolve_phy_port();
  init_infiniband_structs();
  init_mem_reg_funcs();

  LOG_INFO("eRPC RawTransport: Created for ID %u. Device %s, port %d.\n",
           rpc_id, resolve.ib_ctx->device->name, resolve.dev_port_id);
}

void RawTransport::init_hugepage_structures(HugeAlloc *huge_alloc,
                                            uint8_t **rx_ring) {
  this->huge_alloc = huge_alloc;
  this->numa_node = huge_alloc->get_numa_node();

  init_recvs(rx_ring);
  init_sends();
}

// The transport destructor is called after \p huge_alloc has already been
// destroyed by \p Rpc. Deleting \p huge_alloc deregisters and frees all SHM
// memory regions.
//
// We only need to clean up non-hugepage structures.
RawTransport::~RawTransport() {
  LOG_INFO("eRPC RawTransport: Destroying transport for ID %u\n", rpc_id);

  // Destroy QPs and CQs. QPs must be destroyed before CQs.
  if (ibv_destroy_qp(qp)) {
    fprintf(stderr, "eRPC RawTransport: Failed to destroy QP.");
    exit(-1);
  }

  if (ibv_destroy_cq(send_cq)) {
    fprintf(stderr, "eRPC RawTransport: Failed to destroy send CQ.");
    exit(-1);
  }

  if (ibv_destroy_cq(recv_cq)) {
    fprintf(stderr, "eRPC RawTransport: Failed to destroy recv CQ.");
    exit(-1);
  }

  if (ibv_destroy_ah(self_ah)) {
    fprintf(stderr,
            "eRPC RawTransport: Failed to destroy self address handle.");
    exit(-1);
  }

  // Destroy protection domain and device context
  if (ibv_dealloc_pd(pd)) {
    fprintf(stderr,
            "eRPC RawTransport: Failed to deallocate protection domain.");
    exit(-1);
  }

  if (ibv_close_device(resolve.ib_ctx)) {
    fprintf(stderr, "eRPC RawTransport: Failed to close device.");
    exit(-1);
  }
}

struct ibv_ah *RawTransport::create_ah(
    const ib_routing_info_t *ib_rinfo) const {
  struct ibv_ah_attr ah_attr;
  memset(&ah_attr, 0, sizeof(struct ibv_ah_attr));
  ah_attr.is_global = is_roce() ? 1 : 0;
  ah_attr.dlid = is_roce() ? 0 : ib_rinfo->port_lid;
  ah_attr.sl = 0;
  ah_attr.src_path_bits = 0;
  ah_attr.port_num = resolve.dev_port_id;  // Local port

  if (is_roce()) {
    ah_attr.grh.dgid.global.interface_id = ib_rinfo->gid.global.interface_id;
    ah_attr.grh.dgid.global.subnet_prefix = ib_rinfo->gid.global.subnet_prefix;
    ah_attr.grh.sgid_index = 0;
    ah_attr.grh.hop_limit = 1;
  }

  return ibv_create_ah(pd, &ah_attr);
}

void RawTransport::fill_local_routing_info(RoutingInfo *routing_info) const {
  memset(static_cast<void *>(routing_info), 0, kMaxRoutingInfoSize);
  auto *ib_routing_info = reinterpret_cast<ib_routing_info_t *>(routing_info);
  ib_routing_info->port_lid = resolve.port_lid;
  ib_routing_info->qpn = qp->qp_num;
  if (is_roce()) ib_routing_info->gid = resolve.gid;
}

bool RawTransport::resolve_remote_routing_info(
    RoutingInfo *routing_info) const {
  auto *ib_rinfo = reinterpret_cast<ib_routing_info_t *>(routing_info);
  ib_rinfo->ah = create_ah(ib_rinfo);
  return (ib_rinfo->ah != nullptr);
}

void RawTransport::resolve_phy_port() {
  std::ostringstream xmsg;  // The exception message

  // Get the device list
  int num_devices = 0;
  struct ibv_device **dev_list = ibv_get_device_list(&num_devices);
  rt_assert(dev_list != nullptr,
            "eRPC RawTransport: Failed to get InfiniBand device list");

  // Traverse the device list
  int ports_to_discover = phy_port;

  for (int dev_i = 0; dev_i < num_devices; dev_i++) {
    struct ibv_context *ib_ctx = ibv_open_device(dev_list[dev_i]);
    rt_assert(ib_ctx != nullptr,
              "eRPC RawTransport: Failed to open dev " + std::to_string(dev_i));

    struct ibv_device_attr device_attr;
    memset(&device_attr, 0, sizeof(device_attr));
    if (ibv_query_device(ib_ctx, &device_attr) != 0) {
      xmsg << "eRPC RawTransport: Failed to query InfiniBand device "
           << std::to_string(dev_i);
      throw std::runtime_error(xmsg.str());
    }

    for (uint8_t port_i = 1; port_i <= device_attr.phys_port_cnt; port_i++) {
      // Count this port only if it is enabled
      struct ibv_port_attr port_attr;
      if (ibv_query_port(ib_ctx, port_i, &port_attr) != 0) {
        xmsg << "eRPC RawTransport: Failed to query port "
             << std::to_string(port_i) << " on device " << ib_ctx->device->name;
        throw std::runtime_error(xmsg.str());
      }

      if (port_attr.phys_state != IBV_PORT_ACTIVE &&
          port_attr.phys_state != IBV_PORT_ACTIVE_DEFER) {
        continue;
      }

      if (ports_to_discover == 0) {
        // Resolution succeeded. Check if the link layer matches.
        if (is_infiniband() &&
            port_attr.link_layer != IBV_LINK_LAYER_INFINIBAND) {
          throw std::runtime_error(
              "Transport type required is InfiniBand but port link layer is " +
              link_layer_str(port_attr.link_layer));
        }

        if (is_roce() && port_attr.link_layer != IBV_LINK_LAYER_ETHERNET) {
          throw std::runtime_error(
              "Transport type required is RoCE but port link layer is " +
              link_layer_str(port_attr.link_layer));
        }

        // Check the class's constant MTU
        size_t active_mtu = enum_to_mtu(port_attr.active_mtu);
        if (kMTU > active_mtu) {
          throw std::runtime_error("Transport's required MTU is " +
                                   std::to_string(kMTU) + ", active_mtu is " +
                                   std::to_string(active_mtu));
        }

        resolve.device_id = dev_i;
        resolve.ib_ctx = ib_ctx;
        resolve.dev_port_id = port_i;
        resolve.port_lid = port_attr.lid;

        // Resolve and cache the ibv_gid struct for RoCE
        if (is_roce()) {
          int ret = ibv_query_gid(ib_ctx, resolve.dev_port_id, 0, &resolve.gid);
          rt_assert(ret == 0, "Failed to query GID");
        }

        return;
      }

      ports_to_discover--;
    }

    // Thank you Mario, but our port is in another device
    if (ibv_close_device(ib_ctx) != 0) {
      xmsg << "eRPC RawTransport: Failed to close InfiniBand device "
           << ib_ctx->device->name;
      throw std::runtime_error(xmsg.str());
    }
  }

  // If we are here, port resolution has failed
  assert(resolve.ib_ctx == nullptr);
  xmsg << "eRPC RawTransport: Failed to resolve InfiniBand port index "
       << std::to_string(phy_port);
  throw std::runtime_error(xmsg.str());
}

void RawTransport::init_infiniband_structs() {
  assert(resolve.ib_ctx != nullptr && resolve.device_id != -1);
}

void RawTransport::init_mem_reg_funcs() {
  using namespace std::placeholders;
  assert(pd != nullptr);
  reg_mr_func = std::bind(ibv_reg_mr_wrapper, pd, _1, _2);
  dereg_mr_func = std::bind(ibv_dereg_mr_wrapper, _1);
}

void RawTransport::init_recvs(uint8_t **rx_ring) {}

void RawTransport::init_sends() {
  for (size_t i = 0; i < kPostlist; i++) {
    send_wr[i].next = &send_wr[i + 1];
    send_wr[i].wr.ud.remote_qkey = kQKey;
    send_wr[i].opcode = IBV_WR_SEND_WITH_IMM;
    send_wr[i].sg_list = &send_sgl[i][0];
  }
}

}  // End erpc

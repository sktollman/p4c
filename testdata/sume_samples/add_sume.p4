
#include <core.p4>
#include <sume_switch.p4>

/*
  Simple program to add two numbers together
 */

typedef bit<48> EthAddr_t;
typedef bit<32> calcField_t;

#define CALC_TYPE   0x1212

// define opCode types
#define ADD_OP       8w0
#define LOOKUP_OP    8w2

// standard Ethernet header
header Ethernet_h {
    EthAddr_t dstAddr;
    EthAddr_t srcAddr;
    bit<16> etherType;
}

header Calc_h {
    calcField_t op1;
    bit<8> opCode;
    calcField_t op2;
    calcField_t result;
}


// List of all recognized headers
struct Parsed_packet {
    Ethernet_h ethernet;
    Calc_h calc;
}

// user defined metadata: can be used to share information between
// TopParser, TopPipe, and TopDeparser
struct user_metadata_t {
    bit<8>  unused;
}

// digest_data, MUST be 256 bits
struct digest_data_t {
    bit<256> unused;
}

// Parser Implementation
// @Xilinx_MaxPacketRegion(1024) - TODO: figure out if this messes with anything
parser TopParser(packet_in b,
                 out Parsed_packet p,
                 out user_metadata_t user_metadata,
                 out digest_data_t digest_data,
                 inout sume_metadata_t sume_metadata) {
    state start {
        b.extract(p.ethernet);
        user_metadata.unused = 0;
        digest_data.unused = 0;
        transition select(p.ethernet.etherType) {
            CALC_TYPE: parse_calc;
            default: accept;
        }
    }

    state parse_calc {
        b.extract(p.calc);
        transition accept;
    }
}

// match-action pipeline
control TopPipe(inout Parsed_packet p,
                inout user_metadata_t user_metadata,
                inout digest_data_t digest_data,
                inout sume_metadata_t sume_metadata) {

    action swap_eth_addresses() {
        EthAddr_t temp = p.ethernet.dstAddr;
        p.ethernet.dstAddr = p.ethernet.srcAddr;
        p.ethernet.srcAddr = temp;
    }

    action set_output_port() {
        sume_metadata.dst_port = sume_metadata.src_port;
    }

    action set_result(calcField_t data) {
        p.calc.result = data;
    }

    action set_result_default() {
        p.calc.result = 32w0;
    }

    table lookup_table {
        key = { p.calc.op1: exact; }

        actions = {
            set_result;
            set_result_default;
        }
        size = 64;
        default_action = set_result_default;
    }

    apply {

        // bounce packet back to sender
        swap_eth_addresses();
        set_output_port();

        // based on the opCode, set the result or state appropriately
        if (p.calc.opCode == ADD_OP) {
            p.calc.result = p.calc.op1 + p.calc.op2;
        } else if (p.calc.opCode == LOOKUP_OP) {
            lookup_table.apply();
        }

    }
}

// Deparser Implementation
//@Xilinx_MaxPacketRegion(1024)
control TopDeparser(packet_out b,
                    in Parsed_packet p,
                    in user_metadata_t user_metadata,
                    inout digest_data_t digest_data,
                    inout sume_metadata_t sume_metadata) {
    apply {
        b.emit(p.ethernet);
        b.emit(p.calc);
    }
}


// Instantiate the switch
SimpleSumeSwitch(TopParser(), TopPipe(), TopDeparser()) main;


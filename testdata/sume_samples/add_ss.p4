
#include <core.p4>
#include <v1model.p4>

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
struct /*headers*/ Parsed_packet {
    Ethernet_h ethernet;
    Calc_h calc;
}

/*
 * All metadata, globally used in the program, also  needs to be assembed
 * into a single struct. As in the case of the headers, we only need to
 * declare the type, but there is no need to instantiate it,
 * because it is done "by the architecture", i.e. outside of P4 functions
 */

struct metadata {
    calcField_t op1; // just to see
    /* In our case it is empty */
}

/*************************************************************************
 ***********************  P A R S E R  ***********************************
 *************************************************************************/
parser MyParser(/*packet_in packet,
                out headers hdr,*/
                packet_in b,
                out Parsed_packet p,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        b.extract(p.ethernet);
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

/*************************************************************************
 ************   C H E C K S U M    V E R I F I C A T I O N   *************
 *************************************************************************/
control MyVerifyChecksum(inout Parsed_packet p,
                         inout metadata meta) {
    apply { }
}

/*************************************************************************
 **************  I N G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/
control MyIngress(inout Parsed_packet p,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {

    action swap_eth_addresses() {
        EthAddr_t temp = p.ethernet.dstAddr;
        p.ethernet.dstAddr = p.ethernet.srcAddr;
        p.ethernet.srcAddr = temp;
    }

    action set_output_port() {
        standard_metadata.egress_port = standard_metadata.ingress_port;
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
        }  else if (p.calc.opCode == LOOKUP_OP) {
            lookup_table.apply();
        }

    }
}

/*************************************************************************
 ****************  E G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/
control MyEgress(inout Parsed_packet p,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {
    apply { }
}

/*************************************************************************
 *************   C H E C K S U M    C O M P U T A T I O N   **************
 *************************************************************************/

control MyComputeChecksum(inout Parsed_packet p, inout metadata meta) {
    apply { }
}

/*************************************************************************
 ***********************  D E P A R S E R  *******************************
 *************************************************************************/
control MyDeparser(packet_out b, in Parsed_packet p) {
    apply {
        b.emit(p.ethernet);
        b.emit(p.calc);
    }
}

/*************************************************************************
 ***********************  S W I T T C H **********************************
 *************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;


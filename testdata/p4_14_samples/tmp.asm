version:
  version: 1.0.1
  run_id: "d33a8e8ac5382256"
phv ingress:
  bridged_metadata.bridged_metadata_indicator: TB0
  ethernet.dstAddr.0-15: H0
  ethernet.dstAddr.16-47: W0
  ethernet.srcAddr.0-15: TW0(16..31)
  ethernet.srcAddr.16-47: TW2
  ethernet.etherType: TW0(0..15)
  inner_ethernet.dstAddr.0-15: TH0
  inner_ethernet.dstAddr.16-47: TW3
  inner_ethernet.srcAddr.0-15: TW1(16..31)
  inner_ethernet.srcAddr.16-47: TW8
  inner_ethernet.etherType: TW1(0..15)
  bridged_metadata.$valid: B0(0)
  ethernet.$valid: B0(1)
  inner_ethernet.$valid: B0(2)
phv egress:
  eg_intr_md.egress_port: H16(0..8)
  ethernet.dstAddr.0-15: TH6
  ethernet.dstAddr.16-47: TW6
  ethernet.srcAddr.0-15: TW4(16..31)
  ethernet.srcAddr.16-47: TW7
  ethernet.etherType: TW4(0..15)
  inner_ethernet.dstAddr.0-15: TH7
  inner_ethernet.dstAddr.16-47: TW12
  inner_ethernet.srcAddr.0-15: TW5(16..31)
  inner_ethernet.srcAddr.16-47: TW13
  inner_ethernet.etherType: TW5(0..15)
  ethernet.$valid: B16(0)
  inner_ethernet.$valid: B16(1)
parser ingress:
  start: $entry_point.$ingress_tna_entry_point.$ingress_metadata
  multi_write: [ TW0, TW2, B0, H0, W0 ]
  hdr_len_adj: 16
  states:
    $entry_point.$ingress_tna_entry_point.$ingress_metadata:  # from state $entry_point.$ingress_tna_entry_point.$ingress_metadata
      0:
        TB0: 0  # value 0 -> TB0 L[0..7]b: ingress::^bridged_metadata.^bridged_metadata_indicator
        B0: 1  # value 1 -> B0 L[0]b: ingress::^bridged_metadata.$valid
        save: { byte0 : 0 }
        shift: 8
        buf_req: 8
        next: $check_resubmit
    $check_resubmit:  # from state ingress::$check_resubmit
      match: [ byte0 ]
      # - match N[-8]B: cast
      0b0*******:
        buf_req: 0
        next: $phase0
      0b1*******:
        buf_req: 0
        next: $resubmit
    $phase0:  # from state ingress::$phase0
      0:
        shift: 8
        buf_req: 8
        next: $skip_to_packet.start
    $skip_to_packet.start:  # from state $skip_to_packet.start
      0:
        save: { byte0 : 12, byte1 : 13 }
        buf_req: 14
        next: parse_ethernet
    parse_ethernet:  # from state ingress::parse_ethernet
      match: [ byte0, byte1 ]
      # - match N[12..13]B: ethernet.etherType
      value_set pvs0 4:
        field_mapping:
          ethernet.etherType(0..7) : byte0(0..7)
          ethernet.etherType(8..15) : byte1(0..7)
        0..3: W0  # ingress::ethernet.dstAddr.16-47
        4..5: H0  # ingress::ethernet.dstAddr.0-15
        6..9: TW2  # ingress::ethernet.srcAddr.16-47
        10..13: TW0
            # - N[80..95]b -> TW0 L[16..31]b: ingress::ethernet.srcAddr.0-15
            # - N[96..111]b -> TW0 L[0..15]b: ingress::ethernet.etherType
        B0: 2  # value 1 -> B0 L[1]b: ingress::ethernet.$valid
        shift: 14
        buf_req: 14
        next: end
      value_set pvs1 4:
        field_mapping:
          ethernet.etherType(0..7) : byte0(0..7)
          ethernet.etherType(8..15) : byte1(0..7)
        0..3: W0  # ingress::ethernet.dstAddr.16-47
        4..5: H0  # ingress::ethernet.dstAddr.0-15
        6..9: TW2  # ingress::ethernet.srcAddr.16-47
        10..13: TW0
            # - N[80..95]b -> TW0 L[16..31]b: ingress::ethernet.srcAddr.0-15
            # - N[96..111]b -> TW0 L[0..15]b: ingress::ethernet.etherType
        B0: 2  # value 1 -> B0 L[1]b: ingress::ethernet.$valid
        shift: 14
        buf_req: 14
        next: parse_inner_ethernet
      0x****:
        0..3: W0  # ingress::ethernet.dstAddr.16-47
        4..5: H0  # ingress::ethernet.dstAddr.0-15
        6..9: TW2  # ingress::ethernet.srcAddr.16-47
        10..13: TW0
            # - N[80..95]b -> TW0 L[16..31]b: ingress::ethernet.srcAddr.0-15
            # - N[96..111]b -> TW0 L[0..15]b: ingress::ethernet.etherType
        B0: 2  # value 1 -> B0 L[1]b: ingress::ethernet.$valid
        shift: 14
        buf_req: 14
        next: end
    parse_inner_ethernet:  # from state ingress::parse_inner_ethernet
      0:
        0..3: TW3  # ingress::inner_ethernet.dstAddr.16-47
        4..5: TH0  # ingress::inner_ethernet.dstAddr.0-15
        6..9: TW8  # ingress::inner_ethernet.srcAddr.16-47
        10..13: TW1
            # - N[80..95]b -> TW1 L[16..31]b: ingress::inner_ethernet.srcAddr.0-15
            # - N[96..111]b -> TW1 L[0..15]b: ingress::inner_ethernet.etherType
        B0: 4  # value 1 -> B0 L[2]b: ingress::inner_ethernet.$valid
        shift: 14
        buf_req: 14
        next: end
    $resubmit:  # from state $resubmit
      0:
        shift: 8
        buf_req: 8
        next: $skip_to_packet.start
deparser ingress:
  dictionary:
    TB0: B0(0)  # ingress::^bridged_metadata.^bridged_metadata_indicator if ingress::^bridged_metadata.$valid
    W0: B0(1)  # ingress::ethernet.dstAddr.16-47 if ingress::ethernet.$valid
    H0: B0(1)  # ingress::ethernet.dstAddr.0-15 if ingress::ethernet.$valid
    TW2: B0(1)  # ingress::ethernet.srcAddr.16-47 if ingress::ethernet.$valid
    TW0: B0(1)
        # - L[16..31]b: ingress::ethernet.srcAddr.0-15 if ingress::ethernet.$valid
        # - L[0..15]b: ingress::ethernet.etherType if ingress::ethernet.$valid
    TW3: B0(2)  # ingress::inner_ethernet.dstAddr.16-47 if ingress::inner_ethernet.$valid
    TH0: B0(2)  # ingress::inner_ethernet.dstAddr.0-15 if ingress::inner_ethernet.$valid
    TW8: B0(2)  # ingress::inner_ethernet.srcAddr.16-47 if ingress::inner_ethernet.$valid
    TW1: B0(2)
        # - L[16..31]b: ingress::inner_ethernet.srcAddr.0-15 if ingress::inner_ethernet.$valid
        # - L[0..15]b: ingress::inner_ethernet.etherType if ingress::inner_ethernet.$valid
parser egress:
  start: $entry_point.$egress_tna_entry_point.$egress_metadata
  multi_write: [ TH6, TW4, TW6, TW7, B16 ]
  hdr_len_adj: 27
  meta_opt: 8191
  states:
    $entry_point.$egress_tna_entry_point.$egress_metadata:  # from state $entry_point.$egress_tna_entry_point.$egress_metadata
      0:
        0..1: H16  # N[7..15]b -> H16 L[0..8]b: egress::eg_intr_md.egress_port
        save: { byte0 : 27 }
        shift: 27
        buf_req: 28
        next: $check_mirrored
    $check_mirrored:  # from state egress::$check_mirrored
      match: [ byte0 ]
      # - match N[0]B: BFN::LookaheadExpression
      0b****0***:
        buf_req: 0
        next: $bridged_metadata
      0b****1***:
        save: { byte0 : 0 }
        buf_req: 1
        next: $mirrored
    $bridged_metadata:  # from state egress::$bridged_metadata
      0:
        shift: 1
        buf_req: 1
        next: start
    start:  # from state egress::start
      0:
        save: { byte0 : 12, byte1 : 13 }
        buf_req: 14
        next: parse_ethernet
    parse_ethernet:  # from state egress::parse_ethernet
      match: [ byte0, byte1 ]
      # - match N[12..13]B: ethernet.etherType
      value_set pvs0 4:
        field_mapping:
          ethernet.etherType(0..7) : byte0(0..7)
          ethernet.etherType(8..15) : byte1(0..7)
        0..3: TW6  # egress::ethernet.dstAddr.16-47
        4..5: TH6  # egress::ethernet.dstAddr.0-15
        6..9: TW7  # egress::ethernet.srcAddr.16-47
        10..13: TW4
            # - N[80..95]b -> TW4 L[16..31]b: egress::ethernet.srcAddr.0-15
            # - N[96..111]b -> TW4 L[0..15]b: egress::ethernet.etherType
        B16: 1  # value 1 -> B16 L[0]b: egress::ethernet.$valid
        shift: 14
        buf_req: 14
        next: end
      value_set pvs1 4:
        field_mapping:
          ethernet.etherType(0..7) : byte0(0..7)
          ethernet.etherType(8..15) : byte1(0..7)
        0..3: TW6  # egress::ethernet.dstAddr.16-47
        4..5: TH6  # egress::ethernet.dstAddr.0-15
        6..9: TW7  # egress::ethernet.srcAddr.16-47
        10..13: TW4
            # - N[80..95]b -> TW4 L[16..31]b: egress::ethernet.srcAddr.0-15
            # - N[96..111]b -> TW4 L[0..15]b: egress::ethernet.etherType
        B16: 1  # value 1 -> B16 L[0]b: egress::ethernet.$valid
        shift: 14
        buf_req: 14
        next: parse_inner_ethernet
      0x****:
        0..3: TW6  # egress::ethernet.dstAddr.16-47
        4..5: TH6  # egress::ethernet.dstAddr.0-15
        6..9: TW7  # egress::ethernet.srcAddr.16-47
        10..13: TW4
            # - N[80..95]b -> TW4 L[16..31]b: egress::ethernet.srcAddr.0-15
            # - N[96..111]b -> TW4 L[0..15]b: egress::ethernet.etherType
        B16: 1  # value 1 -> B16 L[0]b: egress::ethernet.$valid
        shift: 14
        buf_req: 14
        next: end
    parse_inner_ethernet:  # from state egress::parse_inner_ethernet
      0:
        0..3: TW12  # egress::inner_ethernet.dstAddr.16-47
        4..5: TH7  # egress::inner_ethernet.dstAddr.0-15
        6..9: TW13  # egress::inner_ethernet.srcAddr.16-47
        10..13: TW5
            # - N[80..95]b -> TW5 L[16..31]b: egress::inner_ethernet.srcAddr.0-15
            # - N[96..111]b -> TW5 L[0..15]b: egress::inner_ethernet.etherType
        B16: 2  # value 1 -> B16 L[1]b: egress::inner_ethernet.$valid
        shift: 14
        buf_req: 14
        next: end
    $mirrored:  # from state egress::$mirrored
      match: [ byte0 ]
      # - match N[0]B: (buffer)
      0x**:
        shift: 1
        buf_req: 1
        next: start
deparser egress:
  dictionary:
    TW6: B16(0)  # egress::ethernet.dstAddr.16-47 if egress::ethernet.$valid
    TH6: B16(0)  # egress::ethernet.dstAddr.0-15 if egress::ethernet.$valid
    TW7: B16(0)  # egress::ethernet.srcAddr.16-47 if egress::ethernet.$valid
    TW4: B16(0)
        # - L[16..31]b: egress::ethernet.srcAddr.0-15 if egress::ethernet.$valid
        # - L[0..15]b: egress::ethernet.etherType if egress::ethernet.$valid
    TW12: B16(1)  # egress::inner_ethernet.dstAddr.16-47 if egress::inner_ethernet.$valid
    TH7: B16(1)  # egress::inner_ethernet.dstAddr.0-15 if egress::inner_ethernet.$valid
    TW13: B16(1)  # egress::inner_ethernet.srcAddr.16-47 if egress::inner_ethernet.$valid
    TW5: B16(1)
        # - L[16..31]b: egress::inner_ethernet.srcAddr.0-15 if egress::inner_ethernet.$valid
        # - L[0..15]b: egress::inner_ethernet.etherType if egress::inner_ethernet.$valid
  egress_unicast_port: H16  # L[0..8]b: egress::eg_intr_md.egress_port
stage 0 ingress:
  exact_match dummy_0 0:
    p4: { name: dummy, size: 512 }
    p4_param_order: 
      ethernet.dstAddr: { type: exact, size: 48, full_size: 48, key_name: "ethernet.dstAddr" }
    row: 7
    bus: 0
    column: [ 2, 3, 4, 5, 6 ]
    ways:
      - [0, 0, 0x1, [7, 2], [7, 3]]
      - [0, 1, 0x0, [7, 4]]
      - [0, 2, 0x0, [7, 5]]
      - [0, 3, 0x0, [7, 6]]
    input_xbar:
      exact group 0: { 0: ethernet.dstAddr.16-47, 32: ethernet.dstAddr.0-15 }
      hash 0:
        0..7: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(0..7)
        10..17: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(0..7)
        20..27: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(0..7)
        30..37: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(0..7)
        8..9: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(8..9)
        18..19: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(8..9)
        28..29: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(8..9)
        38..39: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15)) ^ ethernet.dstAddr.0-15(8..9)
        40: random(ethernet.dstAddr.16-47, ethernet.dstAddr.0-15(10..15))
      hash group 0:
        table: [0]
        seed: 0x197bb8457f3
    format: { version(0): 112..115, match(0): [66..71, 32..63 ], version(1): 116..119, match(1): [106..111, 72..103 ] }
    match: [ ethernet.dstAddr.0-15(10..15), ethernet.dstAddr.16-47(0..7), ethernet.dstAddr.16-47(8..15), ethernet.dstAddr.16-47(16..23), ethernet.dstAddr.16-47(24..31) ]
    next: END
    actions:
      noop(0, 0):
      - default_action: { allowed: true }
      NoAction(-1, 0):
      - default_only_action: { allowed: true }
    default_only_action: NoAction

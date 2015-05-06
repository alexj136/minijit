#ifndef ncodegen
#define ncodegen

typedef struct NCodeLinkable NCodeLinkable;
struct NCodeLinkable {
    byte *ncode_buf;
    int ncode_size;
    struct LinkTargetVector *link_targets;
    struct IntRefVector *label_map;
};

/*
 * LinkTargets store information about a jump within native code. Once the ncode
 * addresses of all labels are known, LinkTarget information is used to point
 * jumps to those labels at those addresses.
 * - jump_address: an index into the ncode byte array, at which an immediate
 *   64-bit jump address should exist
 * - label_no: the icode level label number that is the target of the jump
 */
typedef struct LinkTarget LinkTarget;
struct LinkTarget {
    int jump_address;
    int label_no;
};

FORWARD_DECLARE_VECTORABLE(LinkTarget)

typedef struct NCodeLinked NCodeLinked;
struct NCodeLinked {
    byte *ncode_buf;
    int ncode_size;
};

NCodeLinkable *ncode_gen(ICodeOperationVector *ops);
NCodeLinked *ncode_link(NCodeLinkable *ncl);

#endif // ncodegen

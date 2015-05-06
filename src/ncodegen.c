#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"
#include "ncode.h"
#include "ncodegen.h"

#define LinkTarget_free free
#define LinkTarget_eq(a, b) \
        ((a->jump_address == b-> jump_address) && (a->label_no == b->label_no))
DEFINE_VECTORABLE(LinkTarget)

NCodeLinkable *ncode_gen(ICodeOperationVector *ops) {

    // Allocate space to store the generated ncode. Allocate enough space such
    // that if every instruction were the longest type, there would be exactly
    // enough space for the generated code.
    size_t *ncode_buf_size = sizeof(byte) * ((ICodeOperationVector_size(ops) *
            JUMPLINK_x86_64_size) + x86_64_preamble_size);
    byte *ncode_buf = challoc(ncode_buf_size);
    size_t ncode_buf_alloc_idx = 0;
    size_t icode_idx;
    //PREAMBLE!
    for(icode_idx = 0; icode_idx < ICodeOperationVector_size(ops);
            icode_idx++) {

        ICodeOperation *cur_instr = ICodeOperationVector_get(ops, icode_idx);
    }

    if(!ops) ERROR("Null Parameter");
    NOT_IMPLEMENTED;
}

NCodeLinked *ncode_link(NCodeLinkable *ncl) {
    if(!ncl) ERROR("Null Parameter");
    NOT_IMPLEMENTED;
}

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
    if(!ops) ERROR("Null Parameter");
    NOT_IMPLEMENTED;
}

NCodeLinked *ncode_link(NCodeLinkable *ncl) {
    if(!ncl) ERROR("Null Parameter");
    NOT_IMPLEMENTED;
}

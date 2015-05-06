#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "icode.h"
#include "ncode.h"
#include "ncodegen.h"

NCodeLinkable *NCodeLinkable_init(byte *ncode_buf, int ncode_size,
        LinkTargetVector *link_targets, IntRefVector *label_map) {
    NCodeLinkable *ncl = challoc(sizeof(NCodeLinkable));
    ncl->ncode_buf     = ncode_buf;
    ncl->ncode_size    = ncode_size;
    ncl->link_targets  = link_targets;
    ncl->label_map     = label_map;
    return ncl;
}

#define LinkTarget_free free
#define LinkTarget_eq(a, b) \
        ((a->jump_address == b-> jump_address) && (a->label_no == b->label_no))
DEFINE_VECTORABLE(LinkTarget)

LinkTarget *LinkTarget_init(int jump_address, int label_no) {
    LinkTarget *link   = challoc(sizeof(LinkTarget));
    link->jump_address = jump_address;
    link->label_no     = label_no;
    return link;
}

NCodeLinkable *ncode_gen(ICodeOperationVector *ops) {

    // Allocate space to store the generated ncode. Allocate enough space such
    // that if every instruction were the longest type, there would be exactly
    // enough space for the generated code.
    int ncode_buf_size = sizeof(byte) * ((ICodeOperationVector_size(ops) *
            JUMPLINK_x86_64_size) + x86_64_preamble_size);
    byte *ncode_buf = challoc(ncode_buf_size);
    int ncode_buf_alloc_idx = 0;
    int icode_idx;
    LinkTargetVector *link_targets = LinkTargetVector_init();
    IntRefVector *label_map = IntRefVector_init();

    // Include the preamble
    write_x86_64_preamble(ncode_buf + ncode_buf_alloc_idx, NULL, NULL);
    ncode_buf_alloc_idx += x86_64_preamble_size;

    for(icode_idx = 0; icode_idx < ICodeOperationVector_size(ops);
            icode_idx++) {

        ICodeOperation *cur_instr = ICodeOperationVector_get(ops, icode_idx);
        int opc = cur_instr->opc;
        int arg1 = cur_instr->arg1;
        int arg2 = cur_instr->arg2;

        if(opc == MOVE) {
            write_x86_64_MOVE(ncode_buf + ncode_buf_alloc_idx, arg1, arg2);
            ncode_buf_alloc_idx += MOVE_x86_64_size;
        }
        else if(opc == LOAD) {
            write_x86_64_LOAD(ncode_buf + ncode_buf_alloc_idx, arg1, arg2);
            ncode_buf_alloc_idx += LOAD_x86_64_size;
        }
        else if(opc == STORE) {
            write_x86_64_STORE(ncode_buf + ncode_buf_alloc_idx, arg1, arg2);
            ncode_buf_alloc_idx += STORE_x86_64_size;
        }
        else if(opc == ADD) {
            write_x86_64_ADD(ncode_buf + ncode_buf_alloc_idx, arg1, arg2);
            ncode_buf_alloc_idx += ADD_x86_64_size;
        }
        else if(opc == SUB) {
            write_x86_64_SUB(ncode_buf + ncode_buf_alloc_idx, arg1, arg2);
            ncode_buf_alloc_idx += SUB_x86_64_size;
        }
        else if(opc == LOADIMM) {
            write_x86_64_LOADIMM(ncode_buf + ncode_buf_alloc_idx, arg1, arg2);
            ncode_buf_alloc_idx += LOADIMM_x86_64_size;
        }
        else if(opc == JUMP) {
            write_x86_64_JUMP(ncode_buf + ncode_buf_alloc_idx, NULL);
            LinkTargetVector_append(link_targets, LinkTarget_init(
                    ncode_buf_alloc_idx + JUMP_x86_64_address_offset, arg1));
            ncode_buf_alloc_idx += JUMP_x86_64_size;
        }
        else if(opc == JUMPCOND) {
            write_x86_64_JUMPCOND(ncode_buf + ncode_buf_alloc_idx, NULL, arg2);
            LinkTargetVector_append(link_targets, LinkTarget_init(
                    ncode_buf_alloc_idx + JUMPCOND_x86_64_address_offset,
                    arg1));
            ncode_buf_alloc_idx += JUMPCOND_x86_64_size;
        }
        else if(opc == JUMPLINK) {
            write_x86_64_JUMPLINK(ncode_buf + ncode_buf_alloc_idx, NULL);
            LinkTargetVector_append(link_targets, LinkTarget_init(
                    ncode_buf_alloc_idx + JUMPLINK_x86_64_address_offset,
                    arg1));
            ncode_buf_alloc_idx += JUMPLINK_x86_64_size;
        }
        else if(opc == JUMPADDR) {
            write_x86_64_JUMPADDR(ncode_buf + ncode_buf_alloc_idx, arg1);
            ncode_buf_alloc_idx += JUMPADDR_x86_64_size;
        }
        else if(opc == LABEL) {
            IntRefVector_append(label_map, IntRef_init(ncode_buf_alloc_idx));
        }
        else if(opc == HALT) {
            write_x86_64_HALT(ncode_buf + ncode_buf_alloc_idx);
            ncode_buf_alloc_idx += HALT_x86_64_size;
        }
    }

    return NCodeLinkable_init(ncode_buf, ncode_buf_size, link_targets,
            label_map);
}

NCodeLinked *ncode_link(NCodeLinkable *ncl) {
    if(!ncl) ERROR("Null Parameter");
    NOT_IMPLEMENTED;
}

//
// Created by 曹顺 on 2019/2/23.
//

#include "function.h"
namespace hypermind {

    HM_OBJ_HASH(Closure) {
        return 0;
    }
    HM_OBJ_FREE(Closure) {
        return false;
    }
    HM_OBJ_DUMP(Closure) {

    }

    HM_OBJ_HASH(Function) {
        return 0;
    }
    HM_OBJ_FREE(Function) {
        return false;
    }
    HM_OBJ_DUMP(Function) {

    }

    HMFunction::HMFunction(VM *vm, HMModule *module) : HMObject(vm,
                                                                                                                 ObjectType::Function,
                                                                                                                 vm->mFunctionClass),
                                                                                                        instructions(Buffer<HMByte>(vm)),
                                                                                                        constants(Buffer<Value>(vm)) {

    }

    void HMFunction::WriteOpcode(Opcode opcode) {
        instructions.Append((HMByte) opcode);
    }

    void HMFunction::WriteShortOperand(int operand) {
        instructions.Append(static_cast<HMByte>(operand & 0xff)); // 写入 低8位
        instructions.Append(static_cast<HMByte>((operand >> 8) & 0xff)); // 写入 高8位
    }

    void HMFunction::WriteIntOperand(int operand) {
        instructions.Append(static_cast<HMByte>(operand & 0xff)); // 写入 0~7位
        instructions.Append(static_cast<HMByte>((operand >> 8) & 0xff)); // 写入 8~15
        instructions.Append(static_cast<HMByte>((operand >> 16) & 0xff)); // 写入 16~24
        instructions.Append(static_cast<HMByte>((operand >> 24) & 0xff)); // 写入 25~32
    }


    FunctionDebug::FunctionDebug(const String &name) : name(name), line(Vector<HMUINT32>()) {

    }

}

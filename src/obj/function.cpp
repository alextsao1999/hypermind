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
        os << _HM_C(" { func name : ") << debug->name << "  } " << std::endl;

    }

    void HMFunction::WriteOpcode(Opcode opcode) {
        instructions.Append((HMByte) opcode);
    }
    void HMFunction::WriteOpcode(HMByte opcode) {
        instructions.Append(opcode);
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

    void HMFunction::DumpInstruction(int i) {
        HMByte *code = instructions.GetBuffer();
        String str;
#define ReadByte() (code[i++])
        // 小端字节序
#define ReadShort() (i += 2,  (code[i - 1] << 8) | code[i - 2])
#define ReadInt() (i += 4, (code[i - 1] << 24) | (code[i - 2] << 16) | (code[i - 3] << 8) | code[i - 4])
        switch ((Opcode) code[i]) {
            case Opcode::LoadConstant:
                str = _HM_C("LoadConstant");
                break;
            case Opcode::LoadLocalVariable:
                str = _HM_C("LoadLocalVariable");
                break;
            case Opcode::StoreLocalVariable:
                str = _HM_C("StoreLocalVariable");
                break;
            case Opcode::LoadModuleVariable:
                str = _HM_C("LoadModuleVariable");
                break;
            case Opcode::StoreModuleVariable:
                str = _HM_C("Store Module Varible");
                break;
            case Opcode::LoadUpvalue:
                str = _HM_C("Load Upvalue");
                break;
            case Opcode::StoreUpvalue:
                str = _HM_C("Store Upvalue");
                break;
            case Opcode::LoadThisField:
                str = _HM_C("Load This Field");
                break;
            case Opcode::StoreThisField:
                str = _HM_C("Store This Field");
                break;
            case Opcode::LoadField:
                str = _HM_C("Load Field");
                break;
            case Opcode::StoreField:
                str = _HM_C("Store Field");
                break;
            case Opcode::Pop:
                str = _HM_C("Pop");
                break;
            case Opcode::PushNull:
                str = _HM_C("Push Null");
                break;
            case Opcode::PushTrue:
                str = _HM_C("Push True");
                break;
            case Opcode::PushFalse:
                str = _HM_C("Push False");
                break;
            case Opcode::Call:
                str = _HM_C("Call");
                break;
            case Opcode::Call0:
            case Opcode::Call1:
            case Opcode::Call2:
            case Opcode::Call3:
            case Opcode::Call4:
            case Opcode::Call5:
            case Opcode::Call6:
            case Opcode::Call7:
                str = _HM_C("Call");
                str += (std::to_string(code[i] - (HMByte) Opcode::Call0));
                break;
            case Opcode::Super:
                str = _HM_C("Super");
                break;
            case Opcode::Super0:
            case Opcode::Super1:
            case Opcode::Super2:
            case Opcode::Super3:
            case Opcode::Super4:
            case Opcode::Super5:
            case Opcode::Super6:
            case Opcode::Super7:
                str = _HM_C("Super");
                str += (std::to_string(code[i] - (HMByte) Opcode::Super0));
                break;
            case Opcode::Jump:
                str = _HM_C("Jump");
                break;
            case Opcode::Loop:
                str = _HM_C("Loop");
                break;
            case Opcode::JumpIfFalse:
                str = _HM_C("Jump If False");
                break;
            case Opcode::JumpIfTrue:
                str = _HM_C("Jump If True");
                break;
            case Opcode::Add:
                str = _HM_C("Add");
                break;
            case Opcode::Sub:
                str = _HM_C("Sub");
                break;
            case Opcode::Mul:
                str = _HM_C("Mul");
                break;
            case Opcode::Div:
                str = _HM_C("Div");
                break;
            case Opcode::And:
                str = _HM_C("And");
                break;
            case Opcode::Or:
                str = _HM_C("Or");
                break;
            case Opcode::CreateClosure:
                str = _HM_C("Create Closure");
                break;
            case Opcode::CloseUpval:
                str = _HM_C("Close Upval");
                break;
            case Opcode::Return:
                str = _HM_C("Return");
                break;
            case Opcode::CreateClass:
                str = _HM_C("Create Class");
                break;
            case Opcode::Constructor:
                str = _HM_C("Constructor");
                break;
            case Opcode::InstanceMethod:
                str = _HM_C("Instance Method");
                break;
            case Opcode::StaticMethod:
                str = _HM_C("Static Method");
                break;
        }

        hm_cout << str << std::endl;
    }

    FunctionDebug::FunctionDebug(const String &name) : name(name), line(Vector<HMUINT32>()) {

    }

}

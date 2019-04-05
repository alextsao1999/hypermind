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
        os << _HM_C(" { HMFunction(") << sizeof(HMFunction) << _HM_C(") ") << static_cast<const void *>(this)
        << _HM_C("  name : ");
        debug->name.dump(os);
        os << _HM_C("  max stack : ") << maxStackSlotNum << _HM_C("  ");
        os << std::endl << _HM_C("   opcode dump : ") << std::endl;
        DumpAllInstructions(os);
        os << _HM_C(" } ");
    }

    void HMFunction::DumpInstruction(int i) {
        HMByte *code = instructions.data;
        String str;
#define Instruction(v)                 str = _HM_C(v);break;
#define ReadByte() (code[i++])
        // 小端字节序
#define ReadShort() (i += 2,  (code[i] << 8) | (code[i - 1]))
#define ReadInt() (i += 4, (code[i] << 24) | (code[i - 1] << 16) | (code[i - 2] << 8) | code[i - 3])
        switch ((Opcode) code[i]) {
            case Opcode::LoadConstant:
                str = _HM_C("Load_Constant");
                str += _HM_C("_") + std::to_string(ReadShort());
                break;
            case Opcode::LoadLocalVariable:
                Instruction("Load_Local_Variable");
            case Opcode::StoreLocalVariable:
                Instruction("Store_Local_Variable");
            case Opcode::LoadModuleVariable:
                Instruction("Load_Module_Variable");
            case Opcode::StoreModuleVariable:
                Instruction("Store_Module_Varible");
            case Opcode::LoadUpvalue:
                Instruction("Load_Upvalue");
            case Opcode::StoreUpvalue:
                Instruction("Store_Upvalue");
            case Opcode::LoadThisField:
                Instruction("Load_This_Field");
            case Opcode::StoreThisField:
                Instruction("Store_This_Field");
            case Opcode::LoadField:
                Instruction("Load_Field");
            case Opcode::StoreField:
                Instruction("Store_Field");
            case Opcode::Pop:
                Instruction("Pop");
            case Opcode::PushNull:
                Instruction("Push_Null");
            case Opcode::PushTrue:
                Instruction("Push_True");
            case Opcode::PushFalse:
                Instruction("Push_False");
            case Opcode::Call:
                str = _HM_C("Call");
                str += _HM_C("_") + std::to_string(ReadShort()) + _HM_C("_") + std::to_string(ReadShort());
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
                Instruction("Jump");
            case Opcode::Loop:
                Instruction("Loop");
            case Opcode::JumpIfFalse:
                Instruction("Jump_If_False");
            case Opcode::JumpIfTrue:
                Instruction("Jump_If_True");
            case Opcode::Add:
                Instruction("Add");
            case Opcode::Sub:
                Instruction("Sub");
            case Opcode::Mul:
                Instruction("Mul");
            case Opcode::Div:
                Instruction("Div");
            case Opcode::And:
                Instruction("And");
            case Opcode::Or:
                Instruction("Or");
            case Opcode::CreateClosure:
                Instruction("Create_Closure");
            case Opcode::CloseUpvalue:
                Instruction("Close_Upval");
            case Opcode::Return:
                Instruction("Return");
            case Opcode::CreateClass:
                Instruction("Create_Class");
            case Opcode::Constructor:
                Instruction("Constructor");
            case Opcode::BindInstanceMethod:
                Instruction("Instance_Method");
            case Opcode::BindStaticMethod:
                Instruction("Static_Method");
        }
        hm_cout << str << std::endl;
#undef Instruction
#undef ReadByte
#undef ReadShort
#undef ReadInt
    }

    void HMFunction::DumpAllInstructions(Ostream &os) {
        HMByte *code = instructions.data;
        HMInteger index;
        // 使用小端字节序
#define ReadByte() (code[i++])
#define ReadShort() (i += 2,  (code[i] << 8) | (code[i - 1]))
#define ReadInt() (i += 4, (code[i] << 24) | (code[i - 1] << 16) | (code[i - 2] << 8) | code[i - 3])
#define Instruction(v) os << v;
#define ShortArg() os << _HM_C("_") << (int) ReadShort();
#define ConstantShortArg()  index = ReadShort(); \
os << _HM_C("_") << index << _HM_C(" -> "); \
constants[index].dump(os);

#define Finish() os << std::endl; break;
        int i = 0;
        while (i < instructions.count) {
            os << _HM_C("     ");
            switch ((Opcode) code[i]) {
                case Opcode::LoadConstant:
                    Instruction("Load_Constant");
                    ConstantShortArg();
                    Finish();
                case Opcode::LoadLocalVariable:
                    Instruction("Load_Local_Variable");
                    ShortArg();
                    Finish();
                case Opcode::StoreLocalVariable:
                    Instruction("Store_Local_Variable");
                    ShortArg();
                    Finish();
                case Opcode::LoadModuleVariable:
                    Instruction("Load_Module_Variable");
                    ShortArg();
                    Finish();
                case Opcode::StoreModuleVariable:
                    Instruction("Store_Module_Varible");
                    ShortArg();
                    Finish();
                case Opcode::LoadUpvalue:
                    Instruction("Load_Upvalue");
                    ShortArg();
                    Finish();
                case Opcode::StoreUpvalue:
                    Instruction("Store_Upvalue");
                    ShortArg();
                    Finish();
                case Opcode::LoadThisField:
                    Instruction("Load_This_Field");
                    Finish();
                case Opcode::StoreThisField:
                    Instruction("Store_This_Field");
                    Finish();
                case Opcode::LoadField:
                    Instruction("Load_Field");
                    ShortArg();
                    Finish();
                case Opcode::StoreField:
                    Instruction("Store_Field");
                    ShortArg();
                    Finish();
                case Opcode::Pop:
                    Instruction("Pop");
                    Finish();
                case Opcode::PushNull:
                    Instruction("Push_Null");
                    Finish();
                case Opcode::PushTrue:
                    Instruction("Push_True");
                    Finish();
                case Opcode::PushFalse:
                    Instruction("Push_False");
                    Finish();
                case Opcode::Call:
                    Instruction("Call");
                    ShortArg();
                    ShortArg();
                    Finish();
                case Opcode::Call0:
                case Opcode::Call1:
                case Opcode::Call2:
                case Opcode::Call3:
                case Opcode::Call4:
                case Opcode::Call5:
                case Opcode::Call6:
                case Opcode::Call7:
                    Instruction("Call");
                    os << code[i] - (HMByte) Opcode::Call0;
                    ShortArg();
                    Finish();
                case Opcode::Super:
                    Instruction("Super");
                    ShortArg();
                    ShortArg();
                    Finish();
                case Opcode::Super0:
                case Opcode::Super1:
                case Opcode::Super2:
                case Opcode::Super3:
                case Opcode::Super4:
                case Opcode::Super5:
                case Opcode::Super6:
                case Opcode::Super7:
                    Instruction("Super");
                    ShortArg();
                    Finish();
                case Opcode::Jump:
                    Instruction("Jump");
                    ShortArg();
                    Finish();
                case Opcode::Loop:
                    Instruction("Loop");
                    Finish();
                case Opcode::JumpIfFalse:
                    Instruction("Jump_If_False");
                    ShortArg();
                    Finish();
                case Opcode::JumpIfTrue:
                    Instruction("Jump_If_True");
                    ShortArg();
                    Finish();
                case Opcode::Add:
                    Instruction("Add");
                    Finish();
                case Opcode::Sub:
                    Instruction("Sub");
                    Finish();
                case Opcode::Mul:
                    Instruction("Mul");
                    Finish();
                case Opcode::Div:
                    Instruction("Div");
                    Finish();
                case Opcode::And:
                    Instruction("And");
                    Finish();
                case Opcode::Or:
                    Instruction("Or");
                    Finish();
                case Opcode::CreateClosure:
                    Instruction("Create_Closure");
                    ConstantShortArg();
                    Finish();
                case Opcode::CloseUpvalue:
                    Instruction("Close_Upval");
                    Finish();
                case Opcode::Return:
                    Instruction("Return");
                    Finish();
                case Opcode::CreateClass:
                    Instruction("Create_Class");
                    Finish();
                case Opcode::Constructor:
                    Instruction("Constructor");
                    Finish();
                case Opcode::BindInstanceMethod:
                    Instruction("Instance_Method");
                    Finish();
                case Opcode::BindStaticMethod:
                    Instruction("Static_Method");
                    Finish();
                case Opcode::End:
                    Instruction("End");
                    Finish();
            }
            i++;
        }

#undef Instruction
#undef ReadByte
#undef ReadShort
#undef ReadInt

    }
}

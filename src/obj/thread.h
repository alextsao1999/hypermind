//
// Created by 曹顺 on 2019/3/5.
//

#ifndef PROJECT_THREAD_H
#define PROJECT_THREAD_H

#include "function.h"
#define INIT_FRAME_NUM 4
namespace hypermind {
    HM_OBJECT(Thread) {
        Value *stack{};
        Value *sp{};
        HMUINT32 stackCapacity{0};

        Frame *frames{};
        HMUINT32 usedFrameNum{0};   // 已使用的frame数量
        HMUINT32 frameCapacity{INIT_FRAME_NUM};  // frame容量

        HMUpvalue *openUpvalues{nullptr};

        // 调用者
        HMThread *caller{};
        // 错误对象
        Value errorObj{};

        HM_OBJ_CONSTRUCTOR(Thread, HMClosure *closure), frames(vm->Allocate<Frame>(INIT_FRAME_NUM)), stackCapacity(
                CeilToPowerOf2(closure->pFn->maxStackSlotNum + 1)) {
            stack = vm->Allocate<Value>(stackCapacity);
            sp = stack;
            prepare(closure, stack);
        }

        void reset(HMClosure *closure);

        void prepare(HMClosure *closure, Value *start);

        void ensureStack(VM *vm, HMUINT32 needSlots);

        inline void createFrame(VM *vm, HMClosure *closure, int argNum) {
            if (usedFrameNum + 1 > frameCapacity) {
                HMUINT32 newCapacity = frameCapacity * 2;
                HMUINT32 frameSize = sizeof(Frame);
                frames = (Frame *) vm->mGCHeap.MemManger(frames, frameSize * frameCapacity, frameSize * newCapacity);
                frameCapacity = newCapacity;
            }
            auto stackSlots = (HMUINT32) (sp - stack);
            HMUINT32 neededSlots = stackSlots + closure->pFn->maxStackSlotNum;
            ensureStack(vm, neededSlots);
            prepare(closure, sp - argNum);
        }

        void closeUpvalue(Value *lastSlot) {
            HMUpvalue *upvalue = openUpvalues;
            while (upvalue != nullptr && upvalue->localVarPtr >= lastSlot) {
                upvalue->closedUpvalue = *(upvalue->localVarPtr);
                upvalue->localVarPtr = &(upvalue->closedUpvalue);
                upvalue = upvalue->nextUpvalue;
            }
            openUpvalues = upvalue;
        }

        HMUpvalue *createOpenUpvalue(VM *vm, Value *localVarPtr) {
            //如果openUpvalues链表为空就创建
            if (openUpvalues == nullptr) {
                openUpvalues = vm->NewObject<HMUpvalue>(localVarPtr);
                return openUpvalues;
            }

            HMUpvalue *preUpvalue = nullptr;
            HMUpvalue *upvalue = openUpvalues;

            while (upvalue != nullptr && upvalue->localVarPtr > localVarPtr) {
                preUpvalue = upvalue;
                upvalue = upvalue->nextUpvalue;
            }

            if (upvalue != nullptr && upvalue->localVarPtr == localVarPtr) {
                return upvalue;
            }
            auto *newUpvalue = vm->NewObject<HMUpvalue>(localVarPtr);
            if (preUpvalue == nullptr) {
                openUpvalues = newUpvalue;
            } else {
                preUpvalue->nextUpvalue = newUpvalue;
            }
            newUpvalue->nextUpvalue = upvalue;
            return newUpvalue;//返回该结点
        }

        void dump(HMByte *ip) {
            HMInteger index;
            HMByte opcode;
            // 使用小端字节序
#define ReadByte() (*(ip++))
#define ReadShort() (ip += 2,  (ip[-1] << 8) | ip[-2])
#define ReadInt() (ip += 4, (ip[-1] << 24) | (ip[-2] << 16) | (ip[-3] << 8) | ip[-4])
#define Instruction(v) hm_cout << v;
#define ShortArg() hm_cout << _HM_C("_") << (int) ReadShort();
#define ConstantShortArg()  index = ReadShort(); \
hm_cout << _HM_C("_") << index;
#define Finish() hm_cout << std::endl; break;
            opcode = ReadByte();
            switch ((Opcode) opcode) {
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
                    hm_cout << opcode - (HMByte) Opcode::Call0;
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
                    ShortArg();
                    Finish();
                case Opcode::Constructor:
                    Instruction("Constructor");
                    Finish();
                case Opcode::BindInstanceMethod:
                    Instruction("Bind_Instance_Method");
                    ShortArg();
                    Finish();
                case Opcode::BindStaticMethod:
                    Instruction("Bind_Static_Method");
                    ShortArg();
                    Finish();
                case Opcode::End:
                    Instruction("End");
                    Finish();
            }
#undef Instruction
#undef ReadByte
#undef ReadShort
#undef ReadInt
#undef Finish
        }

        void dumpStack(Value *stack, HMUINT32 num) {
            for (int i = 0; i < num; ++i) {
                hm_cout << i << " : ";
                stack[i].dump(hm_cout);
                hm_cout << std::endl;
            }
        }

        void execute(VM *vm);

        HM_OBJ_DECL();

    };

}


#endif //PROJECT_THREAD_H

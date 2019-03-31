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

        Frame *frames{};
        HMUINT32 stackCapacity{0};
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

        void reset(HMClosure *closure) {
            sp = stack;
            openUpvalues = nullptr;
            caller = nullptr;
            errorObj = Value(ValueType::Null);
            usedFrameNum = 0;
            prepare(closure, sp);
        }

        void prepare(HMClosure *closure, Value *start) {
            ASSERT(frameCapacity > usedFrameNum, "frame not enough!!");
            Frame *frame = &(frames[usedFrameNum++]);
            frame->stackStart = start;
            frame->closure = closure;
            frame->ip = closure->pFn->instructions.GetBuffer();
        }

        void ensureStack(VM *vm, HMUINT32 needSlots) {
            if (stackCapacity >= needSlots) {
                return;
            }
            HMUINT32 newStackCapacity = CeilToPowerOf2(needSlots);
            ASSERT(newStackCapacity > stackCapacity, "newStackCapacity error!");
            Value *oldStack = stack;

            HMUINT32 slotSize = sizeof(Value);
            stack = (Value *) vm->MemManger(stack, stackCapacity * slotSize, newStackCapacity * slotSize);
            stackCapacity = newStackCapacity;

            long offset = stack - oldStack;
            if (offset != 0) {
                // 调整各堆栈框架的地址
                HMUINT32 idx = 0;
                while (idx < usedFrameNum) {
                    frames[idx++].stackStart += offset;
                }
                // 调整"open upValue"
                HMUpvalue *upvalue = openUpvalues;
                while (upvalue != nullptr) {
                    upvalue->localVarPtr += offset;
                    upvalue = upvalue->nextUpvalue;
                }
                // 更新栈顶
                sp += offset;
            }

        }

        inline void createFrame(VM *vm, HMClosure *closure, int argNum) {
            if (usedFrameNum + 1 > frameCapacity) {
                HMUINT32 newCapacity = frameCapacity * 2;
                HMUINT32 frameSize = sizeof(Frame);
                frames = (Frame *) vm->MemManger(frames, frameSize * frameCapacity, frameSize * newCapacity);
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
                    Finish();
                case Opcode::Constructor:
                    Instruction("Constructor");
                    Finish();
                case Opcode::InstanceMethod:
                    Instruction("Instance_Method");
                    Finish();
                case Opcode::StaticMethod:
                    Instruction("Static_Method");
                    Finish();
                case Opcode::End:
                    Instruction("End");
                    Finish();
            }
#undef Instruction
#undef ReadByte
#undef ReadShort
#undef ReadInt

        }

        void dumpStack(Value *stack, HMUINT32 num) {
            for (int i = 0; i < num; ++i) {
                hm_cout << i << " : ";
                stack[i].dump(hm_cout);
                hm_cout << std::endl;
            }
        }

        void execute(VM *vm) {
            vm->mCurrentThread = this;
            Frame *curFrame;
            Value *stackStart;
            HMByte *ip;
            HMFunction *fn;
            HMByte opcode;
            //  ------------------------------ 定义宏函数 (使用小端字节序)
#define LoadCurFrame() curFrame = &frames[usedFrameNum - 1]; stackStart = curFrame->stackStart; \
    fn = curFrame->closure->pFn;ip = curFrame->ip;
#define StoreCurFrame() curFrame->ip = ip
#define Push(v) (*sp++ = v)
#define Pop() (*(--sp))
#define Peek(n) (*(sp - n))
#define ReadByte() (*(ip++))
#define ReadShort() (ip += 2,  (ip[-1] << 8) | ip[-2])
#define ReadInt() (ip += 4, (ip[-1] << 24) | (ip[-2] << 16) | (ip[-3] << 8) | ip[-4])
#define Finish() break;
            //  ------------------------------ 加载当前栈帧
            LoadCurFrame();
            //  ------------------------------ 加载完成
            while (true) {
                dump(ip);
                opcode = ReadByte();
                switch ((Opcode) opcode) {
                    case Opcode::LoadConstant:
                        Push(fn->constants[ReadShort()]);
                        Finish();
                    case Opcode::LoadLocalVariable:
                        Push(stackStart[ReadShort()]);
                        Finish();
                    case Opcode::StoreLocalVariable:
                        stackStart[ReadShort()] = Pop();
                        Finish();
                    case Opcode::LoadModuleVariable:
                        ReadShort();
                        Finish();
                    case Opcode::StoreModuleVariable:
                        ReadShort();
                        Finish();
                    case Opcode::LoadUpvalue:
                        ReadShort();
                        Finish();
                    case Opcode::StoreUpvalue:
                        ReadShort();
                        Finish();
                    case Opcode::LoadThisField:
                        Finish();
                    case Opcode::StoreThisField:
                        Finish();
                    case Opcode::LoadField:
                        Finish();
                    case Opcode::StoreField:
                        Finish();
                    case Opcode::Pop:
                        Pop();
                        Finish();
                    case Opcode::PushNull:
                        Push(Value(ValueType::Null));
                        Finish();
                    case Opcode::PushTrue:
                        Push(Value(ValueType::True));
                        Finish();
                    case Opcode::PushFalse:
                        Push(Value(ValueType::False));
                        Finish();
                    case Opcode::Call:
                        ReadShort();
                        ReadShort();
                        Finish();
                    case Opcode::Call0:
                    case Opcode::Call1:
                    case Opcode::Call2:
                    case Opcode::Call3:
                    case Opcode::Call4:
                    case Opcode::Call5:
                    case Opcode::Call6:
                    case Opcode::Call7: {
                        // 将闭包函数弹出
                        Value val = Pop();
                        createFrame(vm, (HMClosure *) val.objval, opcode - (HMByte) Opcode::Call0);
                        ReadShort();
                        StoreCurFrame();
                        LoadCurFrame();
                    }
                        Finish();
                    case Opcode::Super:
                        Finish();
                    case Opcode::Super0:
                    case Opcode::Super1:
                    case Opcode::Super2:
                    case Opcode::Super3:
                    case Opcode::Super4:
                    case Opcode::Super5:
                    case Opcode::Super6:
                    case Opcode::Super7:
                        Finish();
                    case Opcode::Jump:
                        Finish();
                    case Opcode::Loop:
                        Finish();
                    case Opcode::JumpIfFalse:
                        Finish();
                    case Opcode::JumpIfTrue:
                        Finish();
                    case Opcode::Add: {
                        Value op1 = Pop();
                        Value op2 = Pop();
                        Push(Value(op1.intval + op2.intval));
                    }
                        Finish();
                    case Opcode::Sub: {
                        Value op1 = Pop();
                        Value op2 = Pop();
                        Push(Value(op1.intval - op2.intval));
                    }
                        Finish();
                    case Opcode::Mul: {
                        Value op1 = Pop();
                        Value op2 = Pop();
                        Push(Value(op1.intval * op2.intval));
                    }
                        Finish();
                    case Opcode::Div: {
                        Value op1 = Pop();
                        Value op2 = Pop();
                        Push(Value(op1.intval / op2.intval));
                    }
                        Finish();
                    case Opcode::And:
                        Finish();
                    case Opcode::Or:
                        Finish();
                    case Opcode::CreateClosure:
                        // ------------------------
                        // 注意 : 没有对Objval 进行检查
                        // ------------------------
                        Push(Value(vm->NewObject<HMClosure>((HMFunction *) fn->constants[ReadShort()].objval)));
                        Finish();
                    case Opcode::CloseUpvalue:

                        Finish();
                    case Opcode::Return:
                        usedFrameNum--;
                        closeUpvalue(stackStart);
                        // 只有当前没有栈帧的时候线程才结束
                        if (usedFrameNum == 0) {
                            dumpStack(stack, fn->maxStackSlotNum);
                            if (caller == nullptr) {
                                stack[0] = Pop();
                            }
                            sp = stack + 1;
                            return;
                        } else {
                            *stackStart = Pop();
                            sp = stackStart + 1;
                        }
                        LoadCurFrame();
                    case Opcode::CreateClass:
                        Finish();
                    case Opcode::Constructor:
                        Finish();
                    case Opcode::InstanceMethod:
                        Finish();
                    case Opcode::StaticMethod:
                        Finish();
                    case Opcode::End:
                        // 这里不可达
                        Finish();
                }

            }
#undef LoadCurFrame
#undef StoreCurFrame
#undef Push
#undef Pop
#undef Peek
#undef ReadByte
#undef ReadShort
#undef ReadInt
#undef Finish
        }

        HM_OBJ_DECL();

    };

}


#endif //PROJECT_THREAD_H

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
                CeilToPowerOf2(closure->pFn->maxStackSlotNum + 1)), stack(vm->Allocate<Value>(stackCapacity)),
                                                        sp(stack) {
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

        void execute(VM *vm) {
            vm->mCurrentThread = this;
            Frame *curFrame;
            Value *stackStart;
            HMByte *ip;
            HMFunction *fn;
            Opcode opcode;
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
                switch ((Opcode) ReadByte()) {
                    case Opcode::LoadConstant:
                        Finish();
                    case Opcode::LoadLocalVariable:
                        Finish();
                    case Opcode::StoreLocalVariable:
                        Finish();
                    case Opcode::LoadModuleVariable:
                        Finish();
                    case Opcode::StoreModuleVariable:
                        Finish();
                    case Opcode::LoadUpvalue:
                        Finish();
                    case Opcode::StoreUpvalue:
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
                        Push(Value(true));
                        Finish();
                    case Opcode::PushFalse:
                        Finish();
                    case Opcode::Call:
                        Finish();
                    case Opcode::Call0:
                    case Opcode::Call1:
                    case Opcode::Call2:
                    case Opcode::Call3:
                    case Opcode::Call4:
                    case Opcode::Call5:
                    case Opcode::Call6:
                    case Opcode::Call7:
//                        os << code[i] - (HMByte) Opcode::Call0;
//                        ShortArg();
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
                    case Opcode::Add:
                        Finish();
                    case Opcode::Sub:
                        Finish();
                    case Opcode::Mul:
                        Finish();
                    case Opcode::Div:
                        Finish();
                    case Opcode::And:
                        Finish();
                    case Opcode::Or:
                        Finish();
                    case Opcode::CreateClosure:
                        Finish();
                    case Opcode::CloseUpvalue:

                        Finish();
                    case Opcode::Return:
                        usedFrameNum--;
                        closeUpvalue(stackStart);
                        // 只有当前没有栈帧的时候线程才结束
                        if (usedFrameNum == 0) {
                            if (caller == nullptr) {
                                stack[0] = Pop();
                            }
                            sp = stack + 1;
                            return;
                        }
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

        }

        HM_OBJ_DECL();

    };

}


#endif //PROJECT_THREAD_H

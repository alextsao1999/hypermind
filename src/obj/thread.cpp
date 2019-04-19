//
// Created by 曹顺 on 2019/3/5.
//

#include "thread.h"
#include "compiler.h"
#include "value.h"
#include "class.h"
namespace hypermind {
    HM_OBJ_DUMP(Thread) {

    }

    HM_OBJ_HASH(Thread) {
        return 0;
    }
    HM_OBJ_FREE(Thread) {
        vm->Deallocate(stack, sizeof(Value) * stackCapacity);
        vm->Deallocate(frames, sizeof(Frame) * frameCapacity);
        HM_FREE_THIS(Thread);
    }

    void HMThread::closeUpvalue(Value *lastSlot) {
        HMUpvalue *upvalue = openUpvalues;
        while (upvalue != nullptr && upvalue->localVarPtr >= lastSlot) {
            upvalue->closedUpvalue = *(upvalue->localVarPtr);
            upvalue->localVarPtr = &(upvalue->closedUpvalue);
            upvalue = upvalue->nextUpvalue;
        }
        openUpvalues = upvalue;
    }

    void HMThread::execute(VM *vm) {
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
#define PushType(t) sp->type = t;sp++;
#define Pop() (*(--sp))
#define PopPtr() --sp
#define Peek(n) (*(sp - n))
#define PeekPtr(n) (sp - n)
#define ReadByte() (*(ip++))
#define ReadShort() (ip += 2,  (ip[-1] << 8) | ip[-2])
#define ReadInt() (ip += 4, (ip[-1] << 24) | (ip[-2] << 16) | (ip[-3] << 8) | ip[-4])
#define Finish() break;
        //  ------------------------------ 加载当前栈帧
        LoadCurFrame();
        //  ------------------------------ 加载完成
        while (true) {
//            dump(ip, vm, curFrame);
            opcode = ReadByte();
            switch ((Opcode) opcode) {
                case Opcode::LoadConstant:
                    Push(fn->constants[ReadShort()]);
                    Finish();
                case Opcode::LoadLocalVariable:
                    Push(stackStart[ReadShort()]);
                    Finish();
                case Opcode::StoreLocalVariable:
                    stackStart[ReadShort()] = Peek(1);
                    Finish();
                case Opcode::LoadModuleVariable:
                    Push(fn->module->varValues[ReadShort()]);
                    Finish();
                case Opcode::StoreModuleVariable:
                    fn->module->varValues[ReadShort()] = Peek(1);
                    Finish();
                case Opcode::LoadUpvalue:
                    Push(*(curFrame->closure->upvalues[ReadShort()]->localVarPtr));
                    Finish();
                case Opcode::StoreUpvalue:
                    *((curFrame->closure->upvalues[ReadShort()])->localVarPtr) = Peek(1);
                    Finish();
                case Opcode::LoadThisField:
                    {
                        auto *instance = (HMInstance *) stackStart->objval;
                        Push(instance->fields[ReadShort()]);
                    }
                    Finish();
                case Opcode::StoreThisField:
                    {
                        Value *value = PeekPtr(1);
                        auto *instance = (HMInstance *) stackStart->objval;
                        instance->fields[ReadShort()] = *value;
                    }
                    Finish();
                case Opcode::LoadField:
                    {
                        Value *value = PopPtr();
                        auto *instance = (HMInstance *) value->objval;
                        Push(instance->fields[ReadShort()]);
                    }
                    Finish();
                case Opcode::StoreField:
                    {
                        Value *value = PopPtr();
                        auto *instance = (HMInstance *) value->objval;
                        instance->fields[ReadShort()] = Peek(1);
                    }
                    Finish();
                case Opcode::Pop:
                    PopPtr();
                    Finish();
                case Opcode::PushNull:
                    PushType(ValueType::Null);
                    Finish();
                case Opcode::PushTrue:
                    PushType(ValueType::True);
                    Finish();
                case Opcode::PushFalse:
                    PushType(ValueType::False);
                    Finish();
                case Opcode::Call:
                    {
                        int index = ReadShort();
                        int argNum = ReadShort();
                        Value *object = sp - argNum - 1;
                        HMClass *claz = vm->GetValueClass(*object);
                        if (claz->methods[index].type == MethodType::Script) {
                            StoreCurFrame();
                            // 默认将闭包作为第一个参数
                            createFrame(vm, claz->methods[index].fn, argNum + 1);
                            LoadCurFrame();
                        } else if (claz->methods[index].type == MethodType::Primitive) {
                            claz->methods[index].pfn(vm, argNum, object);
                            sp = object + 1;
                        } else if (claz->methods[index].type == MethodType::FunctionCall) {
                            StoreCurFrame();
                            // 默认将闭包作为第一个参数
                            createFrame(vm, (HMClosure *) object->objval, argNum + 1);
                            LoadCurFrame();
                        }
                    }
                    Finish();
                case Opcode::Call0:
                case Opcode::Call1:
                case Opcode::Call2:
                case Opcode::Call3:
                case Opcode::Call4:
                case Opcode::Call5:
                case Opcode::Call6:
                case Opcode::Call7:
                    {
                        int index = ReadShort();
                        int argNum = opcode - (HMByte) Opcode::Call0;
//                        hm_cout << "-----------------------------" << std::endl;
//                        curFrame->closure->pFn->debug->name.dump(hm_cout);
//                        hm_cout << "  ---> " << static_cast<const void *>(sp);
//                        hm_cout << std::endl;
//                        dumpStack(stack, 10);

                        Value *object = sp - argNum - 1;
                        HMClass *claz = vm->GetValueClass(*object);
                        if (claz->methods[index].type == MethodType::Script) {
                            StoreCurFrame();
                            // 默认将闭包作为第一个参数
                            createFrame(vm, claz->methods[index].fn, argNum + 1);
                            LoadCurFrame();
                        } else if (claz->methods[index].type == MethodType::Primitive) {
                            claz->methods[index].pfn(vm, argNum, object);
                            sp = object + 1;
                        } else if (claz->methods[index].type == MethodType::FunctionCall) {
                            StoreCurFrame();
                            // 默认将闭包作为第一个参数
                            createFrame(vm, (HMClosure *) object->objval, argNum + 1);
                            LoadCurFrame();
                        }
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
                case Opcode::CreateClosure:
                    {
                        // ------------------------
                        // 注意 : 没有对Objval 进行检查
                        // ------------------------
                        auto *pfn = (HMFunction *) fn->constants[ReadShort()].objval;
                        auto *closure = vm->NewObject<HMClosure>(pfn);
                        for (int i = 0; i < pfn->upvalueNum; ++i) {
                            if (pfn->upvalues[i].isDirectOuterLocalVar) {
                                closure->upvalues[i] = createOpenUpvalue(vm, stackStart + pfn->upvalues[i].index);
                            } else {
                                closure->upvalues[i] = curFrame->closure->upvalues[pfn->upvalues[i].index];
                            }
                        }
                        Push(Value(closure));
                    }
                    Finish();
                case Opcode::CloseUpvalue:
                    closeUpvalue(sp - 1);
                    PopPtr();
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
                    } else {
                        *stackStart = Pop();
                        sp = stackStart + 1;
                    }
                    LoadCurFrame();
                    Finish();
                case Opcode::CreateClass:
                {
                    Value *superClass = PopPtr();
                    Value *className = PopPtr();
                    auto *claz = vm->NewObject<HMClass>(
                            (HMString *) className->objval, (HMClass *) superClass->objval, ReadShort());
                    Push(claz);
                }
                    Finish();
                case Opcode::CreateInstance:
                {
                    // ------------------------
                    // TODO 注意 : 没有进行检查
                    // ------------------------
                    stackStart[0] = vm->NewObject<HMInstance>((HMClass *) stackStart[0].objval);
                }
                    Finish();
                case Opcode::BindInstanceMethod:
                case Opcode::BindStaticMethod:
                    {
                        int index = ReadShort();
                        Value *closure = PopPtr();
                        Value *value = PeekPtr(1);
//                        curFrame->closure->pFn->symbols[index]
                        auto *claz = (HMClass *) value->objval;
                        if ((Opcode) opcode == Opcode::BindStaticMethod) {
                            claz = claz->classObj;
                        }
                        claz->bind(vm, static_cast<HMUINT32>(index), (HMClosure *) closure->objval);
                }
                    Finish();
                case Opcode::End:
                    // 这里不可达
                    Finish();
                default:
                    break;
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

    void HMThread::reset(HMClosure *closure) {
        sp = stack;
        openUpvalues = nullptr;
        caller = nullptr;
        errorObj = Value(ValueType::Null);
        usedFrameNum = 0;
        prepare(closure, sp);
    }

    void HMThread::prepare(HMClosure *closure, Value *start) {
        ASSERT(frameCapacity > usedFrameNum, "frame not enough!!");
        Frame *frame = &(frames[usedFrameNum++]);
        frame->stackStart = start;
        frame->closure = closure;
        frame->ip = closure->pFn->instructions.data;
    }

    void HMThread::ensureStack(VM *vm, HMUINT32 needSlots) {
        if (stackCapacity >= needSlots) {
            return;
        }
        HMUINT32 newStackCapacity = CeilToPowerOf2(needSlots);
        ASSERT(newStackCapacity > stackCapacity, "newStackCapacity error!");
        Value *oldStack = stack;

        HMUINT32 slotSize = sizeof(Value);
        stack = (Value *) vm->mGCHeap.MemManger(stack, stackCapacity * slotSize, newStackCapacity * slotSize);
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

    HMUpvalue *HMThread::createOpenUpvalue(VM *vm, Value *localVarPtr) {
        // 如果openUpvalues链表为空就创建
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
        return newUpvalue; // 返回该结点
    }

}
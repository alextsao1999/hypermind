//
// Created by 曹顺 on 2019/3/5.
//

#include "thread.h"
#include "value.h"
#include "class.h"
namespace hypermind {
    HM_OBJ_DUMP(Thread) {

    }
    HM_OBJ_HASH(Thread) {
        return 0;
    }
    HM_OBJ_FREE(Thread) {
        return false;
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
            dump(ip, vm, curFrame);
//                hm_cout << "------------------------------" << std::endl;
//                dumpStack(stack, 10);
//                hm_cout << "------------------------------" << std::endl;
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
                    ReadShort();
                    Finish();
                case Opcode::StoreUpvalue:
                    ReadShort();
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

                    Finish();
                case Opcode::StoreField:
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
                    int index = ReadShort();
                    StoreCurFrame();

                    int argNum = opcode - (HMByte) Opcode::Call0;
                    Value *closure = sp - argNum - 1;
                    HMClass *claz = vm->GetValueClass(*closure);
                    if (claz->methods[index].type == MethodType::Script) {

                        // 默认将闭包作为第一个参数
                        createFrame(vm, claz->methods[index].fn, argNum + 1);
                        LoadCurFrame();
                    } else if (claz->methods[index].type == MethodType::Primitive) {
                        claz->methods[index].pfn(vm, closure);
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
                case Opcode::Add: {
                    Value *op1 = PopPtr();
                    Value *op2 = PopPtr();
                    Push(Value(op1->intval + op2->intval));
                }
                    Finish();
                case Opcode::Sub: {
                    Value *op1 = PopPtr();
                    Value *op2 = PopPtr();
                    Push(Value(op1->intval - op2->intval));
                }
                    Finish();
                case Opcode::Mul: {
                    Value op1 = Pop();
                    Value op2 = Pop();
                    Push(Value(op1.intval * op2.intval));
                }
                    Finish();
                case Opcode::Div: {
                    Value *op1 = PopPtr();
                    Value *op2 = PopPtr();
                    Push(Value(op1->intval / op2->intval));
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
                        if (caller == nullptr) {
                            stack[0] = Pop();
                        }
                        sp = stack + 1;
                        return;
                    } else {
                        *stackStart = Pop();
                        sp = stackStart + 1;

                        hm_cout << " --- >>> " << sp - stack << std::endl;
                        dumpStack(stack, 10);
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

                    Value *value = PopPtr();
                    Push(vm->NewObject<HMInstance>((HMClass *) value->objval));
                }
                    Finish();
                case Opcode::BindInstanceMethod:
                {
                    int index = ReadShort();
                    Value *closure = PopPtr();
                    Value *value = PeekPtr(1);
                    auto *claz = (HMClass *) value->objval;
                    claz->methods.set(&vm->mGCHeap, static_cast<HMUINT32>(index),
                                      HMMethod((HMClosure *) closure->objval));
                }
                    Finish();
                case Opcode::BindStaticMethod:
                    ReadShort();
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
}
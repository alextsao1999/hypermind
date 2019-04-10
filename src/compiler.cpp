//
// Created by 曹顺 on 2019/2/25.
//

#include "compiler.h"

#define AST_ENTER() if (flag == CompileFlag::Check) return CompileFlag::Error;compiler->mCurCompileUnit->mLine = line; \
compiler->mCurCompileUnit->mColumn = column;
#define AST_LEAVE() return CompileFlag::Null;
namespace hypermind {
    // 编译语法块
    AST_COMPILE(ASTBlock) {
        AST_ENTER();
        for (auto &stmt : stmts)
            stmt->compile(compiler);
        AST_LEAVE();
    }

    // 编译二元表达式
    AST_COMPILE(ASTBinary) {
        AST_ENTER();
        // Σ(っ °Д °;)っ 这是个异类
        if (op.type == TokenType::Assign) {
            switch (lhs->compile(compiler, CompileFlag::Check)) {
                case CompileFlag::Error:

                    break;
                case CompileFlag::Assign:
                    rhs->compile(compiler, CompileFlag::Null);
                    lhs->compile(compiler, CompileFlag::Assign);
                    break;
                case CompileFlag::Setter:
                    lhs->compile(compiler, CompileFlag::Setter); // 将要Setter的对象和arg加载到栈顶
                    rhs->compile(compiler, CompileFlag::Null); // 加载参数
                    lhs->compile(compiler, CompileFlag::Assign); // EmitCall
                    break;
                default:
                    break;
            }
            return CompileFlag::Null;
        }

        lhs->compile(compiler);
        rhs->compile(compiler);
        // 根据Op 编译相应的Opcode
        switch (op.type) {
            case TokenType::Add:
            case TokenType::Sub:
            case TokenType::Mul:
            case TokenType::Div:
            case TokenType::Arrow:
            case TokenType::Not:
            case TokenType::Equal:
            case TokenType::NotEqual:
            case TokenType::Greater:
            case TokenType::Less:
            case TokenType::GreaterEqual:
            case TokenType::LessEqual:
            case TokenType::Or:
            case TokenType::LogicOr:
            case TokenType::And:
            case TokenType::LogicAnd:
            case TokenType::Mod:
                compiler->mCurCompileUnit->EmitCallSignature(Signature(SignatureType::Method, op.start, op.length), 1);
                break;
            default:
                break;
        }
        AST_LEAVE();
    }

    // 编译字面量
    AST_COMPILE(ASTLiteral) {
        AST_ENTER();
        switch (value.type) {
            case ValueType::Undefined:
                break;
            case ValueType::Null:
                compiler->mCurCompileUnit->EmitPushNull();
                break;
            case ValueType::True:
                compiler->mCurCompileUnit->EmitPushTrue();
                break;
            case ValueType::False:
                compiler->mCurCompileUnit->EmitPushFalse();
                break;
            case ValueType::Integer:
            case ValueType::Float:
            case ValueType::Object:
                HMInteger idx = compiler->mCurCompileUnit->AddConstant(value);
                compiler->mCurCompileUnit->EmitLoadConstant(idx);
                break;
        }
        AST_LEAVE();
    }

    // 编译变量
    AST_COMPILE(ASTVariable) {
        if (flag == CompileFlag::Check) {
            if (var.type == TokenType::KeywordThis)
                return CompileFlag::This;
            return CompileFlag::Assign;
        }
        AST_ENTER();
        Variable variable = compiler->mCurCompileUnit->FindVariable(var);
        if (variable.scopeType == ScopeType::Invalid) {
            // 局部变量不存在 尝试加载this中的Field
            if (compiler->mCurClassInfo != nullptr) {
                // 查找Class Field
                variable.index = compiler->mCurClassInfo->fields.Find(var);
                if (variable.index == -1) {
                    // TODO Error 域不存在
                }

                if (compiler->mCurCompileUnit->mCurClassInfo != nullptr) {
                    // 编译方法
                    if (flag == CompileFlag::Assign) {
                        compiler->mCurCompileUnit->EmitStoreThisField(variable.index);
                    } else {
                        compiler->mCurCompileUnit->EmitLoadThisField(variable.index);
                    }
                } else {
                    // 编译方法中的闭包 将this作为upvalue加载到其中
                    compiler->mCurCompileUnit->LoadThis();
                    // 先将 this 弹出 之后对this进行 取值赋值
                    if (flag == CompileFlag::Assign) {
                        // 此时的栈结构为  值 | this
                        compiler->mCurCompileUnit->EmitStoreField(variable.index);
                    } else {
                        compiler->mCurCompileUnit->EmitLoadField(variable.index);
                    }
                }
            } else {
                // TODO Error 变量不存在
            }
            return CompileFlag::Null;
        }

        if (flag == CompileFlag::Assign) {
            compiler->mCurCompileUnit->EmitStoreVariable(variable);
        } else {
            compiler->mCurCompileUnit->EmitLoadVariable(variable);
        }
        AST_LEAVE();
    }

    // 编译IF
    AST_COMPILE(ASTIfStmt) {
        AST_ENTER();
        AST_LEAVE();
    }

    // 编译While
    AST_COMPILE(ASTWhileStmt) {
        AST_ENTER();
        AST_LEAVE();
    }

    // 编译Continue
    AST_COMPILE(ASTContinueStmt) {
        AST_ENTER();
        AST_LEAVE();
    }

    // 编译Break
    AST_COMPILE(ASTBreakStmt) {
        AST_ENTER();
        AST_LEAVE();
    }

    // 编译Return
    AST_COMPILE(ASTReturnStmt) {
        AST_ENTER();
        if (retvalue == nullptr) {
            compiler->mCurCompileUnit->EmitPushNull();
        } else {
            retvalue->compile(compiler);
        }
        compiler->mCurCompileUnit->EmitReturn();
        AST_LEAVE();
    }

    // 编译List
    AST_COMPILE(ASTList) {
        AST_ENTER();
        for (auto &element : elements)
            element->compile(compiler);
        AST_LEAVE();
    }

    /**
     * 编译变量声明
     * 声明变量的时候 如果初始值为空 则压入Null值 如果 初始值不为空 则 压入初始值
     * @param cu
     */
    AST_COMPILE(ASTVarStmt) {
        AST_ENTER();
        if (value == nullptr) {
            compiler->mCurCompileUnit->EmitPushNull();
        } else {
            value->compile(compiler);
        }
        HMInteger index = compiler->mCurCompileUnit->DeclareVariable(identifier);
        // 局部变量并没有什么变化
        //  如果是模块变量的话 会把局部变量值弹出 存到模块变量中
        compiler->mCurCompileUnit->DefineVariable(index);
        AST_LEAVE();
    }

    AST_COMPILE(ASTParamStmt) {
        AST_ENTER();
        // 声明参数
        compiler->mCurCompileUnit->AddLocalVariable(identifier);
        AST_LEAVE();
    }

    // 编译函数
    AST_COMPILE(ASTFunctionStmt) {
        AST_ENTER();
        HMInteger funIdx = compiler->mCurCompileUnit->DeclareVariable(name);
        // 创建编译单元
#ifdef HMDebug
        //  附上调试信息
        CompileUnit cu = compiler->CreateCompileUnit(new FunctionDebug(name));
#else
        CompileUnit cu = compiler->CreateCompileUnit();
#endif
        compiler->mCurCompileUnit = &cu;
        cu.AddLocalVariable(_HM_C("arg"));
        // 进入作用域
        cu.EnterScope(); // 直接就编译了 不需要离开作用域
        cu.mFn->maxStackSlotNum = cu.mStackSlotNum = cu.mLocalVarNumber;
        params->compile(compiler, CompileFlag::Null); // 编译参数声明
        // 默认传入this 并不包含this
        cu.mFn->argNum = params->elements.size();
        body->compile(compiler); // 编译函数实体
        compiler->LeaveCompileUnit(cu, false);
        cu.EmitEnd();
        // 记录函数上值 创建闭包的时候用
        cu.StoreUpvalue();
        HMInteger index = cu.mOuter->AddConstant(Value(cu.mFn));
        cu.mOuter->EmitCreateClosure(index);
        cu.mOuter->DefineVariable(funIdx);
        AST_LEAVE();

    }

    // 编译类
    AST_COMPILE(ASTClassStmt) {
        AST_ENTER();
        HMInteger classIdx = compiler->mCurCompileUnit->DeclareVariable(name);
        auto *className = compiler->mVM->NewObject<HMString>(name.start, name.length);

        compiler->mCurCompileUnit->EmitLoadConstant(
                compiler->mCurCompileUnit->AddConstant(Value(className)));

        // -------------------------
        // ClassName
        // SuperClass
        // -------------------------

        ClassInfo classInfo;
        classInfo.outer = compiler->mCurClassInfo;
        compiler->mCurClassInfo = &classInfo;

        // 装载Field的符号信息
        for (auto &field : fields) {
            classInfo.fields.Add(&compiler->mVM->mGCHeap, Signature(field->identifier));
        }

        // 加载基类
        if (super.type == TokenType::End) {
            compiler->mCurCompileUnit->LoadModuleVar(_HM_C("Object"));
        } else {
            compiler->mCurCompileUnit->LoadModuleVar(super);
        }

        // 创建Class对象
        compiler->mCurCompileUnit->EmitCreateClass(classInfo.fields.GetCount());

        // 绑定方法
        for (auto &method : methods) {
            method->compile(compiler, CompileFlag::Null);
        }

        for (auto &stc : statics) {
            stc->compile(compiler, CompileFlag::Static);
        }

        // 恢复外层ClassInfo
        compiler->mCurClassInfo = classInfo.outer;
        compiler->mCurCompileUnit->DefineVariable(classIdx);
        AST_LEAVE();
    }

    AST_COMPILE(ASTMethodStmt) {
        AST_ENTER();
#ifdef HMDebug
        //  附上调试信息
        CompileUnit cu = compiler->CreateCompileUnit(new FunctionDebug(name));
#else
        CompileUnit cu = compiler->CreateCompileUnit();
#endif
        cu.mCurClassInfo = compiler->mCurClassInfo;
        compiler->mCurCompileUnit = &cu;
        cu.AddLocalVariable(_HM_C("this"));
        cu.EnterScope();
        cu.mFn->maxStackSlotNum = cu.mStackSlotNum = cu.mLocalVarNumber;
        if (params != nullptr) {
            params->compile(compiler, CompileFlag::Null); // 编译参数声明
            cu.mFn->argNum = cu.mLocalVarNumber - 1;
        }
        body->compile(compiler); // 编译函数实体
        compiler->LeaveCompileUnit(cu, name.type == SignatureType::Constructor);
        cu.EmitEnd();
        // 记录函数上值 创建闭包的时候用
        cu.StoreUpvalue();
        HMInteger index = cu.mOuter->AddConstant(Value(cu.mFn));
        cu.mOuter->EmitCreateClosure(index);

        HMInteger methodIndex = cu.mVM->mAllMethods.EnsureFind(&cu.mVM->mGCHeap, name);
        if (flag == CompileFlag::Static) {
            cu.mOuter->EmitBindStaticMethod(methodIndex);
        } else {
            cu.mOuter->EmitBindInstanceMethod(methodIndex);
        }

        if (name.type == SignatureType::Constructor) {
            // 为构造函数在Meta类中生成静态方法
            CompileUnit newCu = compiler->CreateCompileUnit(new FunctionDebug(name));
            compiler->mCurCompileUnit = &newCu;
            newCu.EmitCreateInstance();
            newCu.EmitCall(methodIndex, cu.mFn->argNum);
            compiler->LeaveCompileUnit(newCu, true);
            newCu.EmitEnd();
            newCu.StoreUpvalue();
            index = newCu.mOuter->AddConstant(Value(newCu.mFn));
            newCu.mOuter->EmitCreateClosure(index);
            newCu.mOuter->EmitBindStaticMethod(methodIndex);
        }
        AST_LEAVE();
    }

    AST_COMPILE(ASTNegativeExpr) {
        AST_ENTER();
        expr->compile(compiler);
        compiler->mCurCompileUnit->EmitCallSignature(Signature(SignatureType::Getter, _HM_C("-")), 0);
        AST_LEAVE();
    }

    AST_COMPILE(ASTNotExpr) {
        AST_ENTER();
        expr->compile(compiler);
        compiler->mCurCompileUnit->EmitCallSignature(Signature(SignatureType::Getter, _HM_C("!")), 0);
        AST_LEAVE();
    }

    AST_COMPILE(ASTArgPostfix) {
        AST_ENTER();
        expr->compile(compiler);
        args->compile(compiler, CompileFlag::Null);
        compiler->mCurCompileUnit->EmitCallSignature(
                Signature(SignatureType::Method, _HM_C("call")), args->elements.size());
        AST_LEAVE();
    }

    AST_COMPILE(ASTDotPostfix) {
        HMBool isThis = expr->compile(compiler, CompileFlag::Check) == CompileFlag::This;

        if (flag == CompileFlag::Check) {
            // 如果expr为this 则返回CompileFlag::Assign 即 rhs先编译  lhs生成store指令
            // Setter 则是调用方法
            return isThis ? CompileFlag::Assign : CompileFlag::Setter;
        }
        AST_ENTER();
        HMInteger index;
        if (isThis) {
            compiler->mCurCompileUnit->LoadThis();
            // 如果为 this 就直接加载
            index = compiler->mCurClassInfo->fields.Find(Signature(name));
            if (flag == CompileFlag::Assign) {
                compiler->mCurCompileUnit->EmitStoreField(index);
            } else {
                compiler->mCurCompileUnit->EmitLoadField(index);
            }
            return CompileFlag::Null;
        }
        if (flag == CompileFlag::Setter) {
            // 如果为setter 先将对象加载到栈顶 之后将rhs加载到栈顶
            expr->compile(compiler);
        } else {
            if (flag != CompileFlag::Assign) {
                // Getter 要将对象加载到栈顶
                expr->compile(compiler);
            }
            compiler->mCurCompileUnit->EmitCallSignature(Signature(
                    flag == CompileFlag::Assign ? SignatureType::Setter : SignatureType::Getter, name.start,
                    name.length), flag == CompileFlag::Assign ? 1 : 0);
        }
        AST_LEAVE();
    }

    AST_COMPILE(ASTMethodPostfix) {
        AST_ENTER();
        expr->compile(compiler);
        // 计算Signature
        Signature signature;
        if (name.type == TokenType::KeywordNew) {
            signature = Signature(SignatureType::Constructor);
        } else {
            signature = Signature(SignatureType::Method, name.start, name.length);
        }
        args->compile(compiler, CompileFlag::Null);
        compiler->mCurCompileUnit->EmitCallSignature(signature, args->elements.size());
        AST_LEAVE();
    }

    AST_COMPILE(ASTSubscriptPostfix) {
        if (flag == CompileFlag::Check)
            return CompileFlag::Setter;
        AST_ENTER();
        if (flag == CompileFlag::Setter) {
            expr->compile(compiler);
            args->compile(compiler, CompileFlag::Null);
        } else {
            if (flag == CompileFlag::Assign) {
                compiler->mCurCompileUnit->EmitCallSignature(Signature(SignatureType::SubscriptSetter),
                                                             args->elements.size() + 1);
            } else {
                // Getter 要把对象和参数加载到栈顶
                expr->compile(compiler);
                args->compile(compiler, CompileFlag::Null);
                compiler->mCurCompileUnit->EmitCallSignature(Signature(SignatureType::Subscript),
                                                             args->elements.size());
            }
        }
        AST_LEAVE();
    }


    HMInteger CompileUnit::DeclareVariable(Signature signature) {
        if (mScopeDepth == -1) {
            return mCurCompiler->mCurModule->add(mVM, signature, Value());
        }

        HMInteger index = FindLocal(signature);
        if (index != -1) {
            // 此前已经存在该局部变量
            return index;
        }
        return AddLocalVariable(signature);
    }

    Variable CompileUnit::FindVariable(Signature signature) {
        Variable var = FindLocalOrUpvalue(signature);
        if (var.scopeType == ScopeType::Invalid) {
            HMInteger index = mCurCompiler->mCurModule->varNames.Find(signature);
            if (index != -1) {
                return Variable(ScopeType::Module, index);
            }
        }
        return var;
    }

    void CompileUnit::LoadModuleVar(Signature signature) {
        HMInteger index = mCurCompiler->mCurModule->varNames.Find(signature);
        EmitLoadVariable(Variable(ScopeType::Module, index));
    }

    void CompileUnit::LoadThis() {
        Variable var = FindLocalOrUpvalue(Signature(_HM_C("this")));
        if (var.scopeType != ScopeType::Invalid) {
            EmitLoadVariable(var);
        }
    }

}
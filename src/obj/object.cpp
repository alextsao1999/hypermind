//
// Created by 曹顺 on 2019/2/19.
//

#include "object.h"
namespace hypermind {
    HMObject::HMObject(hypermind::ObjectType type, hypermind::HMClass *objClass,
                                  hypermind::HMObject *next) : type(type), objClass(objClass), next(next) {}

}

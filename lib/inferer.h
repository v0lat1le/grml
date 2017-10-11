#pragma once

#include "types.h"
#include "ast.h"

namespace grml
{
    Type infer(const Expression& expr);
}

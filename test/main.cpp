/*
 * Typed Lua Wrapping
 * Copyright (C) 2019  Erik Nijenhuis
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "test.h"

Lua lua;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Configure all prototypes for the cross-test lua state
    auto constructorPrototype = PrettyClassPrototypeBuilder("ConstructorExample",
                                                            mk_function(&ConstructorExample::create))
            .build();

    auto getterPrototype = PrettyClassPrototypeBuilder("GetterExample")
            .getter("property", mk_function(&GetterExample::getProperty))
            .build();

    auto setterPrototype = PrettyClassPrototypeBuilder("SetterExample")
            .setter("property", mk_function(&SetterExample::setProperty))
            .build();

    auto methodPrototype = PrettyClassPrototypeBuilder("MethodExample")
            .method("method1", mk_method(&MethodExample::method1))
            .method("method2", mk_method(&MethodExample::method2))
            .method("method3", mk_method(&MethodExample::method3))
            .method("method4", mk_method(&MethodExample::method4))
            .getter("self", mk_function(&MethodExample::getObject))
            .build();

    auto constMethodPrototype = PrettyClassPrototypeBuilder("ConstMethodExample")
            .getter("callMask", mk_function(&MethodExample::getCallMask))
            .build();

    auto numberPrototype = PrettyClassPrototypeBuilder("NumberExample")
            .inverse(mk_function(&NumberExample::inverse))
            .addition(mk_function(&NumberExample::addition))
            .subtraction(mk_function(&NumberExample::subtraction))
            .multiplication(mk_function(&NumberExample::multiplication))
            .division(mk_function(&NumberExample::division))
            .modulo(mk_function(&NumberExample::modulo))
            .power(mk_function(&NumberExample::power))
            .equality(mk_function(&NumberExample::equality))
            .lessThan(mk_function(&NumberExample::lessThan))
            .lessThanOrEqual(mk_function(&NumberExample::lessThanOrEqual))
            .build();

    auto strPrototype = ClassPrototypeBuilder("StrExample")
            .concat(mk_function(&StringExample::concat))
            .build();

    auto classExamplePrototype = PrettyClassPrototypeBuilder("ClassExample")
            .property("number", mk_property(&ClassExample::number))
            .build();

    auto constClassExamplePrototype = PrettyClassPrototypeBuilder("ConstClassExample")
            .getter("number", mk_function(&ClassExample::get))
            .build();

    lua
            .set("noop", FunctionExample::lnoop)
            .set("plus", FunctionExample::lplus)
            .add<ConstructorExample>(constructorPrototype)
            .add<GetterExample>(getterPrototype)
            .add<SetterExample>(setterPrototype)
            .add<MethodExample>(methodPrototype)
            .add<const MethodExample>(constMethodPrototype)
            .add<NumberExample>(numberPrototype)
            .add<StringExample>(strPrototype)
            .add<ClassExample>(classExamplePrototype)
            .add<const ClassExample>(constClassExamplePrototype);

    return RUN_ALL_TESTS();
}

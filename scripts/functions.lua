--
-- Created by IntelliJ IDEA.
-- User: enijenhuis
-- Date: 19-12-2019
-- Time: 11:40
-- To change this template use File | Settings | File Templates.
--

r1 = nil
r3 = nil

function f1()
    r1 = true
end

function f2()
    return true
end

function f3(s1, s2)
    r3 = s1 .. ' ' .. s2
end

function f4(n1, n2)
    return n1 + n2
end

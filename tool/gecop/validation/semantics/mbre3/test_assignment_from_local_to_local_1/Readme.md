# Semantics MBRE-3

This [test](.) is exercising the case [3](../Readme.md) of the semantics rule [MBRE](../../mbre/Readme.md).

### Description

In this test, the source of the assignment is a local variable declared with a reference type but attached to an object with copy semantics. The effect the assignment is the one of cloning this object and attaching it to the target (another local variable declared with a reference type). This test satisfies `MBRE-3`.

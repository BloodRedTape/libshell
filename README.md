# Simple kind-of-shell interpreter
## Because I don't want to write a console menu for every lab

## Examples

Basic command syntax

```
cmd_name arg0 arg1 .. argN
```

List argument syntax

```
[el0, el1, .. , elN]
```


Dictionary argument syntax

```
{name0=value0, name1=value1, .. , nameN=valueN}
```


Real life example

```
> useradd {name=Someone, age=20, job=Manager}

> sort [2, 5, 1, 4, 12]

> merge [1, 5, 3, 2] [2, 8, 7, 9]

> delete Someone

```
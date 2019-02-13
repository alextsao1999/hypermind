# hypermind
hypermind language



## Grammer

It's just like javascript but there is many differences

```javascript
file = import("file")

// foreach element
foods = ["apple", "banana", "seafood"]
foods -> food {
    print("I like eat %(food) very much!")
}

// hashmap
some['my'] = {"test": 'test value'}
print("test value: %(some.my.test)")

// function

add(x, y) = x + y

sub(x, y) = {
    return x + y
}

mul = (x, y) = { return x * y }

// class

People = {
    age = 0
    new() = {
        print("I'm born")
    }
    speak() = {
        print("I am %(age) years old !")
    }
}

// extend
Man : people = {
    who() = {
        print("I'm a man")
    }
    grow(age) = {
    	this.age = age
	}
}
var man = Man()
man.who() // print I'm a man
man.grow(20)
man.speak() // print I am 20 years old !

```


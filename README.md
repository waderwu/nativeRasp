# nativeRasp
nativeRasp that can hook native methods


## Usage

1. build: `gcc nativeRasp.c  -I /usr/lib/jvm/java-8-openjdk-amd64/include -I /usr/lib/jvm/java-8-openjdk-amd64/include/linux/ -fPIC -shared -o libnativerasp.so`
2. load
```java
public class Main {
    static {
        System.load("/tmp/libnativerasp.so");
    }

    public static void main(String[] args) {
        try{
            Runtime.getRuntime().exec("ls -al");
        }catch (Exception e){
            e.printStackTrace();
        }
    }
}
```

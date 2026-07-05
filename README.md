# Slopjective-C
Turns C into an enterprise language by abusing X-Macro generation and Objective-C terminology.

It adds class definitions with single inheritance and inferred virtual methods that can be overridden, as well as a small layer of ARC slop objects implemented using it to get an interesting example (this can be stripped out by inheriting a different base class than Object, as well as removing expressions involving refCount in the generated functions). This stripped version is also provided.

<p align="center">
  <img width="95%" height="95%" alt="Slopjective-C" src="https://github.com/user-attachments/assets/0dc0b678-1612-4f44-8fa1-55ceaa7969e2" />
</p>

LTB Geometry Visualization Server
=================================
[![Travis CI][travis-badge]][travis-link]
[![Codecov][codecov-badge]][codecov-link]
[![MIT License][license-badge]][license-link]
[![Docs][docs-badge]][docs-link]

**[Source][source-code-link]** | **[Documentation][documentation-link]**

A tool for visually debugging geometric applications. Built on a 
[Protobuf][protobuf-link]/[gRPC][grpc-link] service and a 
[Magnum][magnum-link] rendering backend.

### TODO:
* Add global scene traits (like lights)
* Create implicitly convertible vector and matrix classes for use with setters and getters
* Make things intersectable
* Add tests after new architecture is finalized
* Add documentation

### Development

```bash
# All from root dir or things may break

# Before all subtree commands
git remote add -f ltb-whatev git@github.com:LoganBarnes/ltb-whatev.git

# Subtree commands use form: subtree-name remote-name branch-name
git subtree add --prefix ltb-whatev ltb-whatev master --squash
git subtree push --prefix ltb-whatev ltb-whatev master
git subtree pull --prefix ltb-whatev ltb-whatev master --squash
```

Helpful video [tutorial](https://www.youtube.com/watch?v=t3Qhon7burE).


[travis-badge]: https://travis-ci.org/LoganBarnes/ltb-gvs.svg?branch=master
[travis-link]: https://travis-ci.org/LoganBarnes/ltb-gvs
[codecov-badge]: https://codecov.io/gh/LoganBarnes/ltb-gvs/branch/master/graph/badge.svg
[codecov-link]: https://codecov.io/gh/LoganBarnes/ltb-gvs
[license-badge]: https://img.shields.io/badge/License-MIT-blue.svg
[license-link]: https://github.com/LoganBarnes/ltb-gvs/blob/master/LICENSE
[docs-badge]: https://codedocs.xyz/LoganBarnes/ltb-gvs.svg
[docs-link]: https://codedocs.xyz/LoganBarnes/ltb-gvs

[protobuf-link]: https://developers.google.com/protocol-buffers/
[grpc-link]: https://grpc.io/
[magnum-link]: https://magnum.graphics/

[source-code-link]: https://github.com/LoganBarnes/ltb-gvs
[documentation-link]: https://codedocs.xyz/LoganBarnes/ltb-gvs/index.html

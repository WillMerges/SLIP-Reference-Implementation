# SLIP-Reference-Implementation
A simple implementation of the Serial Line Internet Protocol (SLIP) using fixed size buffers.

While IP is in the name, any protocol can be encapsulated, making SLIP a very simple framing protocol.
Any additional features (error detection/correction, flow control, etc.) should be handled by the encapsulated protocol if desired, SLIP only provides framing.

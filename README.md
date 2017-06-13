# Reliable Service Framework

To compile simply call the 'make' command. The framework is compiled with
a testing configuration. After the compilation, you can run the 
components by the following commands:

```
RSF_client -s i
```
runs the client and requests the i-th service, where i belongs to [0, 2].
```
RSF_start_broker
```
runs the broker component and its health checker. By default, the health 
checker expects 3 servers per service but it can be changed in the broker
main.

```
RSF_deployment_unit -s i -n j
```
runs the deployment of the servers, where j servers are deployed for the
 service i. For this testing config j=3 must be used.

The test/ folder contains a set of testing scripts. Each of them must 
be run from the framework folder and simulates a particular situation.

The scripts are the following:
```
client_request_available.sh
```
A client requests a service which is available.
```
client_request_not_available.sh
```
A client requests a service which actually is not running.
```
client_request_one_server_down.sh
```
A client requests a service 5 times while one server has been killed.
```
client_request_two_server_down.sh
```
A client requests a service 5 times while two servers has been killed.
```
client_multiple_requests_available.sh
```
3 clients send a request 5 times for the same service.
```
multiple_requests_multiple_services.sh
```
2 clients send a request 5 times for two different services.
```
kill_broker.sh
```
Kills the broker process and waits its restart.
```
kill_server.sh
```
Kills a server process and waits its restart.

After every script execution each component prints some log information
to a different log file. The logs are located in the folder log/ and are
deleted before executing a new script.

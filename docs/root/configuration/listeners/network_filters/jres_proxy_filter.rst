.. _config_network_filters_jres_proxy:

Jres proxy
============

The jres proxy filter decodes the RPC protocol between jres clients
and servers. the decoded RPC information is converted to metadata.
the metadata includes the basic request ID, request type, serialization type,
and the required service name, method name, parameter name,
and parameter value for routing.

* :ref:`v3 API reference <envoy_v3_api_msg_extensions.filters.network.jres_proxy.v3.jresProxy>`
* This filter should be configured with the name *envoy.filters.network.jres_proxy*.

.. _config_network_filters_jres_proxy_stats:

Statistics
----------

Every configured jres proxy filter has statistics rooted at *jres.<stat_prefix>.* with the
following statistics:

.. csv-table::
  :header: Name, Type, Description
  :widths: 1, 1, 2

  request, Counter, Total requests
  request_twoway, Counter, Total twoway requests
  request_oneway, Counter, Total oneway requests
  request_event, Counter, Total event requests
  request_decoding_error, Counter, Total decoding error requests
  request_decoding_success, Counter, Total decoding success requests
  request_active, Gauge, Total active requests
  response, Counter, Total responses
  response_success, Counter, Total success responses
  response_error, Counter, Total responses that protocol parse error
  response_error_caused_connection_close, Counter, Total responses that caused by the downstream connection close
  response_business_exception, Counter, Total responses that the protocol contains exception information returned by the business layer
  response_decoding_error, Counter, Total decoding error responses
  response_decoding_success, Counter, Total decoding success responses
  response_error, Counter, Total responses that protocol parse error
  local_response_success, Counter, Total local responses
  local_response_error, Counter, Total local responses that encoding error
  local_response_business_exception, Counter, Total local responses that the protocol contains business exception
  cx_destroy_local_with_active_rq, Counter, Connections destroyed locally with an active query
  cx_destroy_remote_with_active_rq, Counter, Connections destroyed remotely with an active query


Implement custom filter based on the jres proxy filter
--------------------------------------------------------

If you want to implement a custom filter based on the jres protocol,
the jres proxy filter like HTTP also provides a very convenient way to expand,
the first step is to implement the DecoderFilter interface, and give the filter named, such as testFilter,
the second step is to add your configuration, configuration method refer to the following sample

.. code-block:: yaml

  filter_chains:
  - filters:
    - name: envoy.filters.network.jres_proxy
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.jres_proxy.v3.JresProxy
        stat_prefix: jres_incomming_stats
        protocol_type: Jres
        serialization_type: Hessian2
        route_config:
          name: local_route
          interface: org.apache.jres.demo.DemoService
          routes:
          - match:
              method:
                name:
                  exact: sayHello
            route:
              cluster: user_service_jres_server
        jres_filters:
        - name: envoy.filters.jres.testFilter
          typed_config:
            "@type": type.googleapis.com/google.protobuf.Struct
            value:
              name: test_service
        - name: envoy.filters.jres.router

// Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.
// Licensed under the Apache License, Version 2.0. See License.txt in the project root for license information.

#include "stdafx.h"
#include "request_sender.h"
#include "http_sender.h"
#include "url_builder.h"

namespace signalr
{
    namespace request_sender
    {
        pplx::task<negotiation_response> negotiate(web_request_factory &request_factory, const web::uri &base_url, const utility::string_t &query_string)
        {
            auto negotiate_url = url_builder::build_negotiate(base_url, query_string);
            auto request = request_factory.create_web_request(negotiate_url);

            return http_sender::get(*request)
                .then([](utility::string_t body)
            {
                auto negotiation_response_json = web::json::value::parse(body);
                negotiation_response response{
                    negotiation_response_json[_XPLATSTR("ConnectionId")].as_string(),
                    negotiation_response_json[_XPLATSTR("ConnectionToken")].as_string(),
                    (int)(negotiation_response_json[_XPLATSTR("DisconnectTimeout")].as_double() * 1000),
                    (int)(negotiation_response_json[_XPLATSTR("KeepAliveTimeout")].as_double() * 1000),
                    negotiation_response_json[_XPLATSTR("ProtocolVersion")].as_string(),
                    negotiation_response_json[_XPLATSTR("TryWebSockets")].as_bool(),
                    (int)(negotiation_response_json[_XPLATSTR("TransportConnectTimeout")].as_double() * 1000)
                };

                return response;
            });
        }
    }
}
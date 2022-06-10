#pragma once

namespace mb
{
    enum class ws_connection_status
    {
        CLOSED,
        CLOSING,
        CONNECTING,
        OPEN
    };
}
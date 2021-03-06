#pragma once
#include <array>
#include <memory>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include "IOManager.hpp"
#include "NatNegPacket.hpp"
#include "ProxyAddressTranslator.h"

namespace CNCOnlineForwarder::NatNeg
{
    class NatNegProxy;
    class InitialPhase;

    class GameConnection : public std::enable_shared_from_this<GameConnection>
    {
    private:
        struct PrivateConstructor {};
    public:
        using Strand = IOManager::StrandType;
        using EndPoint = boost::asio::ip::udp::endpoint;
        using Socket = WithStrand<boost::asio::ip::udp::socket>;
        using Timer = WithStrand<boost::asio::steady_timer>;
        using NatNegPlayerID = NatNegPlayerID;
        using PacketView = NatNegPacketView;
        using Buffer = std::unique_ptr<char[]>;

        static constexpr auto description = "GameConnection";

        static std::shared_ptr<GameConnection> create
        (
            const IOManager::ObjectMaker& objectMaker,
            const std::weak_ptr<NatNegProxy>& proxy,
            const std::weak_ptr<ProxyAddressTranslator>& addressTranslator,
            const EndPoint& server,
            const EndPoint& clientPublicAddress
        );

        GameConnection
        (
            PrivateConstructor,
            const IOManager::ObjectMaker& objectMaker,
            const std::weak_ptr<NatNegProxy>& proxy,
            const std::weak_ptr<ProxyAddressTranslator>& addressTranslator,
            const EndPoint& server,
            const EndPoint& clientPublicAddress
        );

        const EndPoint& getClientPublicAddress() const noexcept;

        void handlePacketToServer(const PacketView packet);

        void handleCommunicationPacketFromServer
        (
            const PacketView packet,
            const EndPoint& communicationAddress
        );

    private:

        void extendLife();

        void prepareForNextPacketFromClient();

        void prepareForNextPacketToClient();

        void handlePacketFromServer(Buffer buffer, const std::size_t size);

        void handleCommunicationPacketFromServerInternal
        (
            const PacketView packet,
            const EndPoint& communicationAddress
        );

        void handlePacketFromRemotePlayer
        (
            Buffer buffer, 
            const std::size_t size, 
            const EndPoint& from
        );

        void handlePacketToRemotePlayer
        (
            Buffer buffer, 
            const std::size_t size, 
            const EndPoint& from
        );

        Strand strand;
        std::weak_ptr<NatNegProxy> proxy;
        std::weak_ptr<ProxyAddressTranslator> addressTranslator;
        EndPoint server;
        EndPoint clientPublicAddress;
        EndPoint clientRealAddress;
        EndPoint remotePlayer;
        Socket publicSocketForClient;
        Socket fakeRemotePlayerSocket;
        Timer timeout;
    };
}


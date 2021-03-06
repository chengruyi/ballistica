// Released under the MIT License. See LICENSE for details.

#ifndef BALLISTICA_GAME_CONNECTION_CONNECTION_TO_CLIENT_H_
#define BALLISTICA_GAME_CONNECTION_CONNECTION_TO_CLIENT_H_

#include <map>
#include <string>
#include <vector>

#include "ballistica/game/connection/connection.h"

namespace ballistica {

/// Connection to a party client if we're the host.
class ConnectionToClient : public Connection {
 public:
  explicit ConnectionToClient(int id);
  ~ConnectionToClient() override;
  void Update() override;
  void HandleMessagePacket(const std::vector<uint8_t>& buffer) override;
  void HandleGamePacket(const std::vector<uint8_t>& buffer) override;
  auto id() const -> int { return id_; }

  // More efficient than dynamic_cast (hmm do we still want this?).
  virtual auto GetAsUDP() -> ConnectionToClientUDP*;
  void SetController(ClientControllerInterface* c);
  auto GetPlayerProfiles() const -> PyObject* { return player_profiles_.get(); }
  auto build_number() const -> int { return build_number_; }
  void SendScreenMessage(const std::string& s, float r = 1.0f, float g = 1.0f,
                         float b = 1.0f);
  auto token() const -> const std::string& { return token_; }
  void HandleMasterServerClientInfo(PyObject* info_obj);

  /// Return the public id for this client. If they have not been verified
  /// by the master-server, returns an empty string.
  auto peer_public_account_id() const -> const std::string& {
    return peer_public_account_id_;
  }

  /// Return whether this client is an admin. Will only return true once their
  /// account id has been verified by the master server.
  auto IsAdmin() const -> bool;

 private:
  virtual auto ShouldPrintIncompatibleClientErrors() const -> bool;
  // Returns a spec for this client that incorporates their player names
  // or their peer name if they have no players.
  auto GetCombinedSpec() -> PlayerSpec;
  auto GetClientInputDevice(int remote_id) -> ClientInputDevice*;
  void Error(const std::string& error_msg) override;
  std::string our_handshake_player_spec_str_;
  std::string our_handshake_salt_;
  std::string peer_public_account_id_;
  ClientControllerInterface* controller_ = nullptr;
  std::map<int, ClientInputDevice*> client_input_devices_;
  millisecs_t last_hand_shake_send_time_ = 0;
  int id_ = -1;
  int build_number_ = 0;
  bool got_client_info_ = false;
  bool kick_voted_ = false;
  bool kick_vote_choice_ = false;
  std::string token_;
  std::string peer_hash_;
  PythonRef player_profiles_;
  bool got_info_from_master_server_ = false;
  std::vector<millisecs_t> last_chat_times_;
  millisecs_t next_kick_vote_allow_time_ = 0;
  millisecs_t chat_block_time_ = 0;
  millisecs_t last_remove_player_time_ = -99999;
  int next_chat_block_seconds_ = 10;
  friend class Game;
};

}  // namespace ballistica

#endif  // BALLISTICA_GAME_CONNECTION_CONNECTION_TO_CLIENT_H_

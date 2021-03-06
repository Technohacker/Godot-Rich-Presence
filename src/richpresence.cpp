#include "richpresence.h"
#include <stdlib.h>
#include <stdio.h>

using namespace godot;

/**
 */
static RichPresence* self = nullptr;


/**
 */
RichPresence::~RichPresence() {
	Discord_Shutdown();
	self = nullptr;
}

void RichPresence::_init() {}

/**
 */
void RichPresence::_register_methods() {
	//
	register_method("init", &RichPresence::init);
	register_method("clear", &RichPresence::clear);
	register_method("update", &RichPresence::update);
	register_method("reply", &RichPresence::reply);
	register_method("run_callbacks", &RichPresence::runCallbacks);
	register_method("shutdown", &RichPresence::shutdown);
	
	// READY
	Dictionary args;
	args[Variant("user")] = Variant(Variant::DICTIONARY);
	register_signal<RichPresence>("ready", args);
	
	// DISCONNECTED
	args.clear();
	args[Variant("error_code")] = Variant(Variant::INT);
	args[Variant("message")] = Variant(Variant::STRING);
	register_signal<RichPresence>("disconnected", args);

	// ERROR
	args.clear();
	args[Variant("error_code")] = Variant(Variant::INT);
	args[Variant("message")] = Variant(Variant::STRING);
	register_signal<RichPresence>("error", args);
	
	// JOIN GAME
	args.clear();
	args[Variant("secret")] = Variant(Variant::STRING);
	register_signal<RichPresence>("join_game", args);
	
	// JOIN REQUEST
	args.clear();
	args[Variant("request")] = Variant(Variant::DICTIONARY);
	register_signal<RichPresence>("join_request", args);
	
	// SPECTATE GAME
	args.clear();
	args[Variant("secret")] = Variant(Variant::STRING);
	register_signal<RichPresence>("spectate_game", args);
}


/** DISCORD CALLBACKS **/
void RichPresence::onReady(const DiscordUser* discordUser) {
	Dictionary user;
	user[Variant("user_id")] = String(discordUser->userId);
	user[Variant("username")] = String(discordUser->username);
	user[Variant("discriminator")] = String(discordUser->discriminator);
	user[Variant("avatar")] = String(discordUser->avatar);
	self->emit_signal("ready", user);
}

void RichPresence::onDisconnected(int code, const char* message) {
	self->emit_signal("disconnected", code, message);
}

void RichPresence::onError(int code, const char* message) {
	self->emit_signal("error", code, message);
}

void RichPresence::onJoin(const char* secret) {
	self->emit_signal("join_game", secret);
}

void RichPresence::onJoinRequest(const DiscordUser* request) {
	Dictionary userRequest;
	userRequest[Variant("user_id")] = String(request->userId);
	userRequest[Variant("username")] = String(request->username);
	userRequest[Variant("discriminator")] = String(request->discriminator);
	userRequest[Variant("avatar")] = String(request->avatar);
	self->emit_signal("join_request", userRequest);
}

void RichPresence::onSpectateGame(const char* secret) {
	self->emit_signal("spectate_game", secret);
}
/** DISCORD CALLBACKS **/


/**
 */
void RichPresence::init(Dictionary initialize) {
	self = this;

	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));

	handlers.ready = RichPresence::onReady;
	handlers.disconnected = RichPresence::onDisconnected;
	handlers.errored = RichPresence::onError;
	handlers.joinGame = RichPresence::onJoin;
	handlers.joinRequest = RichPresence::onJoinRequest;
	handlers.spectateGame = RichPresence::onSpectateGame;

	CharString buff = (initialize["app_id"].operator String()).ascii();

	int autoRegister = 1;
	const char* steamId = NULL;
	const char* appId = buff.get_data();

	if (initialize.has("auto_register")) {
		autoRegister = ((int)initialize["auto_register"]);
	}

	if (initialize.has("steam_id")) {
		buff = (initialize["steam_id"].operator String()).ascii();
		steamId = buff.get_data();
	}

	Discord_Initialize(appId, &handlers, autoRegister, steamId);
}

/**
 */
void RichPresence::clear() {
	Discord_ClearPresence();
}
/**
 */
void RichPresence::runCallbacks() {
	Discord_RunCallbacks();
}
/**
 *
 */
void RichPresence::shutdown() {
	Discord_Shutdown();
}

/**
 */
void RichPresence::reply(String userId, int response) {
	int resp = DISCORD_REPLY_NO;
	CharString buff = userId.ascii();
	
	switch (response) {
		case 1:
			resp = DISCORD_REPLY_YES;
			break;
		case 2:
			resp = DISCORD_REPLY_IGNORE;
			break;
	}

	Discord_Respond(buff.get_data(), resp);
}

/**
 *	https://discordapp.com/developers/docs/rich-presence/how-to
 */
void RichPresence::update(Dictionary presence) {
	
	//	Not initialized
	if (!self) { return; }

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));


	//	the user's current party status
	//	idk if is necessary or optional "discord-rpc/src/serialization.cpp 108"
	if (presence.has("state")) {
		discordPresence.state = (presence["state"].operator String()).alloc_c_string();
	}


	//	what the player is currently doing
	//	idk if is necessary or optional "discord-rpc/src/serialization.cpp 109"
	if (presence.has("details")) {
		discordPresence.details = (presence["details"].operator String()).alloc_c_string();
	}


	//	OPTIONAL
	//	discord-rpc/src/serialization.cpp 111
	//	epoch seconds for game start - including will show time as "elapsed"
	if (presence.has("start_timestamp")) {
		discordPresence.startTimestamp = (int64_t)presence["start_timestamp"];
	}


	//	OPTIONAL
	//	discord-rpc/src/serialization.cpp 111
	//	epoch seconds for game end - including will show time as "remaining"
	if (presence.has("end_timestamp")) {
		discordPresence.endTimestamp = (int64_t)presence["end_timestamp"];
	}

	
	//	OPTIONAL
	//	name of the uploaded image for the large profile artwork	
	if (presence.has("large_image_key")) {
		discordPresence.largeImageKey = (presence["large_image_key"].operator String()).alloc_c_string();
	}


	//	OPTIONAL
	//	tooltip for the largeImageKey
	if (presence.has("large_image_text")) {
		discordPresence.largeImageText = (presence["large_image_text"].operator String()).alloc_c_string();
	}


	//	OPTIONAL
	//	name of the uploaded image for the small profile artwork
	if (presence.has("small_image_key")) {
		discordPresence.smallImageKey = (presence["small_image_key"].operator String()).alloc_c_string();
	}


	//	OPTIONAL
	//	tootltip for the smallImageKey
	if (presence.has("small_image_text")) {
		discordPresence.smallImageText = (presence["small_image_text"].operator String()).alloc_c_string();
	}


	// OPTIONAL
	// discord-rpc/src/serialization.cpp 136
	if (presence.has("party_id") || presence.has("party_size") || presence.has("party_max")) {

		//	id of the player's party, lobby, or group
		if (presence.has("party_id")) {
			discordPresence.partyId = (presence["party_id"].operator String()).alloc_c_string();
		}

		//	current size of the player's party, lobby, or group
		//	maximum size of the player's party, lobby, or group
		if (presence.has("party_size") && presence.has("party_max")) {
			discordPresence.partySize = (int)presence["party_size"];
			discordPresence.partyMax = (int)presence["party_max"];
		}
	}


	//	OPTIONAL
	//	unique hashed string for Spectate button
	//	discord-rpc/src/serialization.cpp 147
	if (presence.has("spectate_secret")) {
		discordPresence.spectateSecret = (presence["spectate_secret"].operator String()).alloc_c_string();
	}


	//	OPTIONAL
	//	unique hashed string for chat invitations and Ask to Join
	//	discord-rpc/src/serialization.cpp 147
	if (presence.has("join_secret")) {
		discordPresence.joinSecret = (presence["join_secret"].operator String()).alloc_c_string();	
	}

	Discord_UpdatePresence(&discordPresence);
}
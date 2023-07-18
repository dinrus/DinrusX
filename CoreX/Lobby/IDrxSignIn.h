// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
#pragma once

<DinrusX/CoreX/Containers/DrxArray.h>
<DinrusX/CoreX/String/DrxString.h>
<DinrusX/CoreX/Lobby/IDrxLobby.h> // <> required for Interfuscator

//! User credential identifiers used by GetUserCredentials task.
enum EDrxLobbyUserCredential
{
	eCLUC_Email,
	eCLUC_Password,
	eCLUC_UserName,
	eCLUC_Invalid
};

struct SUserCredential
{
	SUserCredential()
		: type(eCLUC_Invalid),
		value()
	{}

	SUserCredential(const SUserCredential& src)
		: type(src.type),
		value(src.value)
	{}

	SUserCredential(EDrxLobbyUserCredential srcType, const string& srcValue)
		: type(srcType),
		value(srcValue)
	{}

	~SUserCredential()
	{}

	const SUserCredential& operator=(const SUserCredential& src)
	{
		type = src.type;
		value = src.value;

		return *this;
	}

	EDrxLobbyUserCredential type;
	string                  value;
};

typedef DynArray<SUserCredential> TUserCredentials;

//! \param taskID				  Task ID allocated when the function was executed.
//! \param error				  Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function.
//! \param user           local user index
//! \param pArg					  Pointer to application-specified data.
typedef void (* DrxSignInCallback)(DrxLobbyTaskID taskID, EDrxLobbyError error, uint32 user, void* pCbArg);

//! \param taskID			    Task ID allocated when the function was executed
//! \param error			    Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
//! \param user           local user index
//! \param pName          If email and password are correct for existing account, the user name for that account
//! \param pArg				    Pointer to application-specified data
typedef void (* DrxSignInCreateUserCallback)(DrxLobbyTaskID taskID, EDrxLobbyError error, uint32 user, const char* pName, void* pArg);

//! \param taskID			    Task ID allocated when the function was executed
//! \param error			    Error code - eCLE_Success if the function succeeded or an error that occurred while processing the function
//! \param user           local user index
//! \param pCredentials   Pointer to array of user credentials
//! \param pArg				    Pointer to application-specified data
typedef void (* DrxSignInGetUserCredentialsCallback)(DrxLobbyTaskID taskID, EDrxLobbyError error, uint32 user, DynArray<SUserCredential>* pCredentials, void* pArg);

struct IDrxSignIn
{
	// <interfuscator:shuffle>
	virtual ~IDrxSignIn() {}

	//! Sign a user in.
	//! \param user				    local user index
	//! \param credentials    credentials of user
	//! \param pTaskID		    lobby task ID
	//! \param pCB				    callback
	//! \param pCBArg			    callback argument
	//! \return			    eCLE_Success if task started successfully, otherwise a failure code
	virtual EDrxLobbyError SignInUser(uint32 user, const TUserCredentials& credentials, DrxLobbyTaskID* pTaskID, DrxSignInCallback pCb, void* pCbArg) = 0;

	//! Sign a user out.
	//! \param user				    local user index
	//! \param pTaskID		    lobby task ID
	//! \param pCB				    callback
	//! \param pCBArg			    callback argument
	//! \return			    eCLE_Success if task started successfully, otherwise a failure code
	virtual EDrxLobbyError SignOutUser(uint32 user, DrxLobbyTaskID* pTaskID, DrxSignInCallback pCb, void* pCbArg) = 0;

	//! Create a new user.
	//! \param user				    local user index
	//! \param credentials    credentials of new user
	//! \param pTaskID		    lobby task ID
	//! \param pCB				    callback
	//! \param pCBArg			    callback argument
	//! \return			    eCLE_Success if task started successfully, otherwise a failure code
	virtual EDrxLobbyError CreateUser(uint32 user, const TUserCredentials& credentials, DrxLobbyTaskID* pTaskID, DrxSignInCreateUserCallback pCb, void* pCbArg) = 0;

	//! Create and sign in a new user.
	//! \param user				    local user index
	//! \param credentials    credentials of new user
	//! \param pTaskID		    lobby task ID
	//! \param pCB				    callback
	//! \param pCBArg			    callback argument
	//! \return			    eCLE_Success if task started successfully, otherwise a failure code
	virtual EDrxLobbyError CreateAndSignInUser(uint32 user, const TUserCredentials& credentials, DrxLobbyTaskID* pTaskID, DrxSignInCreateUserCallback pCb, void* pCbArg) = 0;

	//! Get the given user's credentials.
	//! \param user				    local user index
	//! \param pTaskID		    lobby task ID
	//! \param pCB				    callback
	//! \param pCBArg			    callback argument
	//! \return			    eCLE_Success if task started successfully, otherwise a failure code
	virtual EDrxLobbyError GetUserCredentials(uint32 user, DrxLobbyTaskID* pTaskID, DrxSignInGetUserCredentialsCallback pCB, void* pCBArg) = 0;

	//! Cancel a task
	//! \param lTaskID		     lobby task ID
	virtual void CancelTask(DrxLobbyTaskID lTaskID) = 0;
	// </interfuscator:shuffle>
};

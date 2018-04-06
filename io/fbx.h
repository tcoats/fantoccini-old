#ifndef _fbx_h
#define _fbx_h

// auto fbxid = ECS::NewID();
// auto scene = (FbxScene*)Bus::Emit(Procedure::LoadFbx, fbxid, "tower.fbx");
// cout << put_time(gmtime(&t), "%FT%TZ") << " Fbx "
//   << "HELLO" << "\n";
// ECS::Delete(fbxid);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wnull-dereference"
#include <fbxsdk.h>
#pragma clang diagnostic pop

#endif

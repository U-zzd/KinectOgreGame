

#ifndef __NuiDataType_h_
#define __NuiDataType_h_

namespace NuiManager 
{
	/*
		Enum data types so the system will not depend directly to Kinect API
	*/

	//-------------------------------------------------------------------------------------
	// https://msdn.microsoft.com/en-us/library/microsoft.kinect.jointtype.aspx
	typedef enum NuiJointIndex
	{
		SPINE_BASE,
		SPINE_MID,
		NECK,
		HEAD,
		SHOULDER_LEFT,
		ELBOW_LEFT,
		WRIST_LEFT,
		HAND_LEFT,
		SHOULDER_RIGHT,
		ELBOW_RIGHT,
		WRIST_RIGHT,
		HAND_RIGHT,
		HIP_LEFT,
		KNEE_LEFT,
		ANKLE_LEFT,
		FOOT_LEFT,
		HIP_RIGHT,
		KNEE_RIGHT,
		ANKLE_RIGHT,
		FOOT_RIGHT,
		SPINE_SHOULDER
	} NuiJointIndex;
	
	//-------------------------------------------------------------------------------------
	typedef enum NuiSkeletonPositionTrackingState
	{	JOINT_NOT_TRACKED = 0,
		JOINT_INFERRED = 1,
		JOINT_TRACKED = 2 
	} NuiSkeletonPositionTrackingState;

	//-------------------------------------------------------------------------------------
	typedef enum NuiSkeletonTrackingState
	{	SKELETON_NOT_TRACKED = 0,
		SKELETON_POSITION_ONLY = 1,
		SKELETON_TRACKED = 2 
	} NuiSkeletonTrackingState;

}

#endif
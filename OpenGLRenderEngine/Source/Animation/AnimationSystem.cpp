#include "AnimationSystem.h"

//===================================
// KeyFrame
//===================================
KeyFrame::KeyFrame(double time, const vector<glm::vec3>& boneTranslation, const vector<glm::quat>& boneRotation)
 :m_time(time), m_boneTranslation(boneTranslation), m_boneRotation(boneRotation)
{
}



//===================================
// Animation
//===================================
Animation::Animation(const AnimationData& animation)
	:m_boneNameToIndex(animation.m_nodeNameToIndexMap), m_duration(animation.m_duration), m_ticksPerSecond(animation.m_ticksPerSecond)
{
	unsigned int boneSize = animation.m_node.size();
	unsigned int maxFrame = boneSize > 0 ? animation.m_node[0].m_rotationFrame.size() : 0;
	m_keyFrames.reserve(maxFrame);
	for(unsigned int i=0; i<maxFrame; i++)
	{
		double frameTime = animation.m_node[0].m_rotationFrame[i].first;
		vector<glm::vec3> translationList(boneSize);
		vector<glm::quat> rotationList(boneSize);

		for(unsigned int j=0; j<boneSize; j++)
		{
			auto& tempRot = animation.m_node[j].m_rotationFrame[i].second;
			rotationList[j].w = tempRot.m_w;
			rotationList[j].x = tempRot.m_x;
			rotationList[j].y = tempRot.m_y;
			rotationList[j].z = tempRot.m_z;

			auto& tempPos = animation.m_node[j].m_positionFrame[i].second;
			translationList[j].x = tempPos.m_x;
			translationList[j].y = tempPos.m_y;
			translationList[j].z = tempPos.m_z;
		}

		m_keyFrames.push_back(KeyFrame(frameTime, translationList, rotationList));
	}
}

unsigned int Animation::getBoneIndex(const string& boneName) const
{
	auto iter = m_boneNameToIndex.find(boneName);
	return iter == m_boneNameToIndex.end() ? ~0 : iter->second;
}


//===================================
// Bone
//===================================
Bone::Bone(const SkeletonNodeData& node)
  :m_name(node.m_name), 
   m_parentID(node.m_parentID),
   m_childID(node.m_childNameList),
   m_transform(glm::make_mat4(node.m_transform.m_m16))
{
}

//===================================
// Skeleton
//===================================

Skeleton::Skeleton(const SkeletonData& skeletonData)
	:m_inverseTransform(glm::make_mat4(skeletonData.m_inverseTransform.m_m16))
{
	unsigned int boneCount = skeletonData.m_nodes.size();
	m_bone.reserve(boneCount);
	for(unsigned int i=0; i<boneCount; i++)
		m_bone.push_back(skeletonData.m_nodes[i]);

	unsigned int animCount = skeletonData.m_animation.size();
	m_animation.reserve(animCount);
	for(unsigned int i=0; i<animCount; i++)
		m_animation.push_back(Animation(skeletonData.m_animation[i]));

}
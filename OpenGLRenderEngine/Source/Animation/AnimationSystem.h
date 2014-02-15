#pragma once
#include "../TypeDef.h"
#include "../File/MeshData.h"
#include "../Graphics/GraphicsGL.h"

class Bone
{
	public:
	Bone(const SkeletonNodeData& node);

	//private:
	string m_name;
	unsigned int m_parentID;
	vector<unsigned int> m_childID;
	glm::mat4 m_transform;
};

class KeyFrame
{
	public:
	KeyFrame(double time,
			 const vector<glm::vec3>& boneTranslation, 
			 const vector<glm::quat>& boneRotation);

	inline double getTime() const { return m_time; }
	inline unsigned int getBoneCount() const { return m_boneTranslation.size(); }

	inline const glm::vec3& getBoneTranslation(unsigned int boneID) const 
	{
		return m_boneTranslation[boneID];
	}

	inline const glm::quat& getBoneRotation(unsigned int boneID) const 
	{
		return m_boneRotation[boneID];
	}


	private:
	double m_time;
	vector<glm::vec3> m_boneTranslation;
	vector<glm::quat> m_boneRotation;
};

class Animation
{
	public:
	Animation(const AnimationData& animation);

	const string& getName() const { return m_name; }
	double getDuration() const { return m_duration; }
	double getTicksPerSecond() const { return m_ticksPerSecond; }
	unsigned int getKeyFrameCount() const { return m_keyFrames.size(); }
	const KeyFrame& getKeyFrame(unsigned int n) const { return m_keyFrames[n]; }

	unsigned int getBoneIndex(const string& boneName) const;

	private:
	string m_name;
	double m_duration;
	double m_ticksPerSecond;

	map<string,unsigned int> m_boneNameToIndex;
	vector<KeyFrame> m_keyFrames;
};

class Skeleton
{
	public:
	Skeleton(const SkeletonData& skeletonData);

	inline unsigned int getAnimationCount() const { 
		m_animation.size(); 
	}

	inline const Animation& getAnimation(const unsigned int nAnim) const {
		return m_animation[nAnim];
	}

	inline const glm::mat4& getInverseTransform() const {
		return m_inverseTransform;
	}

	inline const Bone& getBoneNode(unsigned int n) const {
		return m_bone[n];
	}

	private:
	glm::mat4 m_inverseTransform;
	vector<Bone> m_bone;
	vector<Animation> m_animation;
};

struct AnimationComponent
{
	AnimationComponent(unsigned int animationIndex, unsigned int startFrameIndex);
	unsigned int m_animationIndex;
	unsigned int m_startFrameIndex;
	unsigned int m_endFrameIndex;
	double m_remainingTime;
};
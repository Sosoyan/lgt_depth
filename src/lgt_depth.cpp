#include <ai.h>
#include <vector>
#include <algorithm>

AI_SHADER_NODE_EXPORT_METHODS(lgt_depth_methods);

enum params
{
	p_write_light_aovs,
	p_invert,
	p_scale,
};

struct ShaderData
{
	std::vector<std::vector<AtNode*>> lgt_groups;
};

node_parameters
{
	AiParameterBool("write_light_aovs", true);
	AiParameterBool("invert", false);
	AiParameterFlt("scale", 1.f);
}

node_initialize
{
	ShaderData* data = new ShaderData;
	AiNodeSetLocalData(node, data);
}

node_update
{
	ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
	AtUniverse* universe = AiRenderSessionGetUniverse(render_session);
	AtNodeIterator* lgt_iter = AiUniverseGetNodeIterator(universe, AI_NODE_LIGHT);

	std::vector<AtString> groups;
	data->lgt_groups = std::vector<std::vector<AtNode*>>();

	while (!AiNodeIteratorFinished(lgt_iter))
	{
		AtNode* lp = AiNodeIteratorGetNext(lgt_iter);
		AtString name = AiNodeGetStr(lp, AtString("aov"));

		std::vector<AtString>::iterator it = std::find(groups.begin(), groups.end(), name);

		if (it != groups.end())
		{
			int64_t index = std::distance(groups.begin(), it);
			data->lgt_groups[index].push_back(lp);
		}
		else
		{
			groups.push_back(name);
			data->lgt_groups.push_back(std::vector<AtNode*>{ lp });
		}
	}

	AiNodeIteratorDestroy(lgt_iter);
}

node_finish
{
	if (AiNodeGetLocalData(node))
	{
		ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
		data->lgt_groups = std::vector<std::vector<AtNode*>>();
		AiNodeSetLocalData(node, NULL);
		delete data;
	}
}

shader_evaluate
{
	ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);

	bool write_light_aovs = AiShaderEvalParamBool(p_write_light_aovs);
	bool invert = AiShaderEvalParamBool(p_invert);
	float scale = AiShaderEvalParamFlt(p_scale);

	AtRGB result = AtRGB(0.f, 0.f, 0.f);

	for (int i = 0; i < data->lgt_groups.size(); ++i)
	{
		AtRGB lgt_group = AtRGB(0.f, 0.f, 0.f);

		std::vector<AtNode*>& lights = data->lgt_groups[i];

		for (int j = 0; j < lights.size(); ++j)
		{
			AtMatrix lgt_mat = AiNodeGetMatrix(lights[j], AtString("matrix"));
			AtVector lgt_pos = AtVector(lgt_mat[3][0],
										lgt_mat[3][1],
										lgt_mat[3][2]);

			float dist = AiV3Length(sg->P - lgt_pos) * scale;

			if (invert)
			{
				if (dist != 0)
					dist = 1.f / dist;

				lgt_group.r = (j == 0) ? dist : AiMax(lgt_group.r, dist);
			}
			else
			{
				lgt_group.r = (j == 0) ? dist : AiMin(lgt_group.r, dist);
			}
		}

		if (write_light_aovs)
		{
			AtString aov = AiNodeGetStr(lights[0], AtString("aov"));
			AiAOVSetRGB(sg, AtString(aov), lgt_group);
		}

		result += lgt_group;
	}

	sg->out.RGB() = result;
}

node_loader
{
   if (i > 0)
	  return false;

   node->methods = lgt_depth_methods;
   node->output_type = AI_TYPE_RGB;
   node->name = "lgt_depth";
   node->node_type = AI_NODE_SHADER;

#ifdef _WIN32
   strcpy_s(node->version, AI_VERSION);
#else
   strcpy(node->version, AI_VERSION);
#endif

   return true;
}

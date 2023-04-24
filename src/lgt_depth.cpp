#include <ai.h>

AI_SHADER_NODE_EXPORT_METHODS(lgt_depth_methods);

enum params
{
	p_write_light_aovs,
	p_scale,
};

node_parameters
{ 
	AiParameterBool("write_light_aovs", true);
	AiParameterFlt("scale", 1.0f);
}

node_initialize
{
}

node_update
{
}

node_finish
{
}

shader_evaluate
{
	bool write_light_aovs = AiShaderEvalParamBool(p_write_light_aovs);
	float scale = AiShaderEvalParamFlt(p_scale);

	AiLightsPrepare(sg);
	AtLightSample ls;

	AtRGB result = AtRGB(0.0f, 0.0f, 0.0f);
	
	for (int i=0; i < sg->nlights; i++)
	{
		AtNode* lp = sg->lights[i];

		AtMatrix lgt_mat = AiNodeGetMatrix(lp, AtString("matrix"));
		AtVector lgt_pos = AtVector(lgt_mat[3][0], 
									lgt_mat[3][1], 
									lgt_mat[3][2]);

		float dist = AiV3Length(lgt_pos - sg->P);
		
		AtRGB lgt_depth = AtRGB(dist, dist, dist);
		
		if (write_light_aovs)
		{
			AtString aov = AiNodeGetStr(lp, AtString("aov"));
			AiAOVSetRGB(sg, AtString(aov), lgt_depth);
		}

		result += lgt_depth;
	}

// 	while (AiLightsGetSample(sg, ls))
// 	{
// 		float dist = ls.Ldist * scale;
		
// 		AtRGB lgt_depth = AtRGB(dist, dist, dist);
	 
// 		if (write_light_aovs)
// 		{
// 			AtString aov = AiNodeGetStr(ls.Lp, AtString("aov"));
// 			AiAOVSetRGB(sg, AtString(aov), lgt_depth);
// 		}

		result += lgt_depth;
	}

	sg->out.RGB() = result;
}

node_loader
{
   if (i > 0)
      return false;

   node->methods     = lgt_depth_methods;
   node->output_type = AI_TYPE_RGB;
   node->name        = "lgt_depth";
   node->node_type   = AI_NODE_SHADER;
   strcpy_s(node->version, AI_VERSION);
   return true;
} 
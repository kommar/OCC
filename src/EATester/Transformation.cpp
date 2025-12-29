#include "Transformation.h"

#include <algorithm>

using namespace std;


template <class TGenotype, class TFenotype>
CTransformation<TGenotype, TFenotype>::CTransformation(CEvaluation<TFenotype> *pcEvaluation)
{
	pc_evaluation = pcEvaluation;
}//CTransformation<TGenotype, TFenotype>::CTransformation(CEvaluation<TFenotype> *pcEvaluation)

template <class TGenotype, class TFenotype>
CTransformation<TGenotype, TFenotype>::~CTransformation()
{

}//CTransformation<TGenotype, TFenotype>::~CTransformation()

template <class TGenotype, class TFenotype>
TFenotype * CTransformation<TGenotype, TFenotype>::pcTransform(TGenotype *pcGenotype)
{
	TFenotype *pc_fenotype = pc_create_fenotype(pcGenotype);
	vTransform(pcGenotype, pc_fenotype);

	return pc_fenotype;
}//TFenotype * CTransformation<TGenotype, TFenotype>::pcTransform(TGenotype *pcGenotype)

template <class TGenotype, class TFenotype>
TGenotype * CTransformation<TGenotype, TFenotype>::pcInverselyTransform(TFenotype *pcFenotype)
{
	TGenotype *pc_genotype = pc_create_genotype(pcFenotype);
	vInverselyTransform(pcFenotype, pc_genotype);

	return pc_genotype;
}//TGenotype * CTransformation<TGenotype, TFenotype>::pcInverselyTransform(TFenotype *pcFenotype)


template <class TGenotypeFenotype>
CUniformTransformation<TGenotypeFenotype>::CUniformTransformation(CEvaluation<TGenotypeFenotype> *pcEvaluation)
	: CTransformation<TGenotypeFenotype, TGenotypeFenotype>(pcEvaluation)
{

}//CUniformTransformation<TGenotypeFenotype>::CUniformTransformation(CEvaluation<TGenotypeFenotype> *pcEvaluation)


template <class TGenotype, class TFenotype>
CTransformation<TGenotype, TFenotype> * TransformationUtils::pcGetTransformation(CEvaluation<TFenotype> *pcEvaluation, istream *psSettings, CError *pcError)
{
	CTransformation<TGenotype, TFenotype> *pc_transformation = nullptr;

	size_t i_genotype_type_hash_code = typeid(TGenotype).hash_code();
	size_t i_fenotype_type_hash_code = typeid(TFenotype).hash_code();

	if (i_genotype_type_hash_code == i_fenotype_type_hash_code)
	{
		pc_transformation = (CTransformation<TGenotype, TFenotype>*)new CUniformTransformation<TFenotype>(pcEvaluation);
	}//if (i_genotype_type_hash_code == i_fenotype_type_hash_code)

	*pcError = pc_transformation->eConfigure(psSettings);

	return pc_transformation;
}//CTransformation<TGenotype, TFenotype> * TransformationUtils::pcGetTransformation(CEvaluation<TFenotype> *pcEvaluation, istream *psSettings, CError *pcError)


template class CTransformation<CRealCoding, CRealCoding>;

template CTransformation<CRealCoding, CRealCoding> * TransformationUtils::pcGetTransformation<CRealCoding, CRealCoding>(CEvaluation<CRealCoding>*, istream*, CError*);
#pragma once
#include "bias_act_dropout.h"
#include "bias_add_transform_20314.h"
#include "bias_dropout_residual.h"
#include "concat3_dim1.h"
#include "dropout.h"
#include "layer.h"
#include "layer_normalize.h"
#include "linear.h"
#include "softmax.h"
#include "strided_batch_gemm.h"
#include "transform_0213.h"

namespace lightseq {

template <class T1, class T2> class DecEncAttentionLayer : public Layer {
private:
  // operators
  LayerNormalizeOp<T1, T2> *_attn_ln = nullptr;
  LinearOp<T1, T2> *_q_linear = nullptr;
  BiasAddTrans20314<T1, T2> *_bias_add_transform_20314_q = nullptr;
  StridedBatchGemmOp<T1, T2> *_attn_scores = nullptr;
  SoftmaxOp<T1, T2> *_softmax = nullptr;
  DropoutOp<T1, T2> *_attn_prob_dropout = nullptr;
  StridedBatchGemmOp<T1, T2> *_attn_context = nullptr;
  Transform0213OP<T1, T2> *_transform_0213 = nullptr;
  LinearOp<T1, T2> *_attn_out_linear = nullptr;
  BiasDropoutResOp<T1, T2> *_attn_dropout = nullptr;

  // parameters
  Variable *_attn_qw;
  Variable *_attn_qb;
  Variable *_attn_ow;
  Variable *_attn_ob;
  Variable *_attn_nw;
  Variable *_attn_nb;

  // shape related
  size_t _batch_dim;
  size_t _batch_heads;
  size_t _batch_tokens;
  size_t _layer_id;
  size_t _max_batch_tokens;
  size_t _max_seq_len;
  size_t _hidden_size;
  size_t _heads;
  bool _is_pre_ln;

public:
  DecEncAttentionLayer(size_t layer_id, size_t max_batch_tokens,
                       size_t max_seq_len, size_t hidden_size, size_t num_heads,
                       float attn_prob_dropout_ratio,
                       float hidden_output_dropout_ratio, bool is_pre_ln);

  virtual ~DecEncAttentionLayer() {}

  Variable *operator()(Variable *inp, Variable *enc_mask, Variable *enc_k,
                       Variable *enc_v);

  void before_forward(int batch_size, int src_seq_len, int trg_seq_len);

  void before_backward();

  size_t load_para_and_grad(const T1 *para_ptr, T2 *grad_ptr);

  int load_params(const std::vector<const T1 *> &para_vec, int offset);
};

template class DecEncAttentionLayer<float, float>;
#ifdef LIGHTSEQ_cuda
template class DecEncAttentionLayer<__half, __half>;
#endif

template <class T1, class T2>
using DecEncAttentionLayerPtr = std::shared_ptr<DecEncAttentionLayer<T1, T2>>;

} // namespace lightseq
struct VSOutput
{
    float4 svpos : SV_POSITION; // ���_�V�F�[�_�[���痈�����W
    float4 color : COLOR; // ���_�V�F�[�_�[���痈���F
    float2 uv : TEXCOORD; // ���_�V�F�[�_�[���痈��uv
};

SamplerState smp : register(s0); // �T���v���[
Texture2D _MainTex : register(t0); // �e�N�X�`��

float4 PSmain(VSOutput input) : SV_TARGET
{
    return _MainTex.Sample(smp, input.uv);
}
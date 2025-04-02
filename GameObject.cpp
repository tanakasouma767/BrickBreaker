#include "GameObject.h"
#include "App.h"

// 四角形の頂点データ生成関数
vector<Vertex> CreateRectangleVertices(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color) 
{
	vector<Vertex> vertices =
	{
		{ { pos.x - size.x / 2.0f, pos.y - size.y / 2.0f, 0.0f }, color, { 0.0f, 1.0f } }, // 左下
		{ { pos.x + size.x / 2.0f, pos.y - size.y / 2.0f, 0.0f }, color, { 1.0f, 1.0f } }, // 右下
		{ { pos.x - size.x / 2.0f, pos.y + size.y / 2.0f, 0.0f }, color, { 0.0f, 0.0f } }, // 左上
		{ { pos.x + size.x / 2.0f, pos.y + size.y / 2.0f, 0.0f }, color, { 1.0f, 0.0f } } // 右上
	};

	return vertices;
}

// 円の頂点データ生成関数
vector<Vertex> CreateCircleVertices(XMFLOAT2 pos, XMFLOAT2 size, XMFLOAT4 color, int segments) 
{
	vector<Vertex> vertices;

	const float PI = 3.14159265359f;

	// 中心点
	vertices.push_back({{ pos.x, pos.y, 0.0f }, color});

	// 円周上の点を計算
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * PI * i / segments; // 角度
		float x = size.x / 2 * cosf(theta);
		float y = size.y / 2 * sinf(theta);
		vertices.push_back({{ pos.x + x,  pos.y + y, 0.0f }, color});
	}

	return vertices;
}

// 円のインデックスデータ生成関数
vector<uint32_t> CreateCircleIndices(int segments) 
{
	vector<uint32_t> indices;

	for (int i = 1; i <= segments; ++i) {
		indices.push_back(0);       // 中心点
		indices.push_back(i);       // 現在の円周上の点
		indices.push_back(i + 1);   // 次の円周上の点
	}

	return indices;
}

GameObject::GameObject(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path)
	: position(pos), size(sz), color(col), offset(pos), texPath(path)
{
	// 変換行列
	auto eyePos = XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f); // 視点の位置
	auto targetPos = XMVectorZero(); // 視点を向ける座標
	auto upward = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // 上方向を表すベクトル
	auto fov = XMConvertToRadians(37.5); // 視野角
	auto aspect = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT); // アスペクト比


	// 変換行列用定数バッファー
	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		constantBuffer[i] = new ConstantBuffer(sizeof(Transform));
		if (!constantBuffer[i]->IsValid())
		{
			printf("変換行列用定数バッファの生成に失敗\n");
		}

		// 変換行列の登録
		auto ptr = constantBuffer[i]->GetPtr<Transform>();
		ptr->World = XMMatrixIdentity();
		ptr->View = XMMatrixLookAtRH(eyePos, targetPos, upward);
		ptr->Proj = XMMatrixPerspectiveFovRH(fov, aspect, 0.3f, 1000.0f);
	}

	// テクスチャ用ディスクリプタヒープ
	descriptorHeap = new DescriptorHeap();
	auto mainTex = Texture2D::Get(texPath);
	auto handle = descriptorHeap->Register(mainTex);
	materialHandles = handle;
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		delete constantBuffer[i];
	}
	delete descriptorHeap;
}


void GameObject::Draw(RootSignature* rootSignature, PipelineState* pipelineState) 
{
	auto currentIndex = g_Engine->CurrentBackBufferIndex(); // 現在のフレーム番号を取得する
	auto commandList = g_Engine->CommandList(); // コマンドリスト
	auto materialHeap = descriptorHeap->GetHeap(); // ディスクリプタヒープ

	auto vbView = vertexBuffer->View();
	auto ibView = indexBuffer->View();

	commandList->SetGraphicsRootSignature(rootSignature->Get());
	commandList->SetPipelineState(pipelineState->Get());
	commandList->SetGraphicsRootConstantBufferView(0, constantBuffer[currentIndex]->GetAddress());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetIndexBuffer(&ibView);

	commandList->SetDescriptorHeaps(1, &materialHeap); // 使用するディスクリプタヒープをセット
	commandList->SetGraphicsRootDescriptorTable(1, materialHandles->HandleGPU); // ディスクリプタテーブルをセット

	commandList->DrawIndexedInstanced(indices.size(), 1, 0, 0, 0);
}

BoundingBox GameObject::GetBoundingBox() 
{
	DirectX::BoundingBox bbox;
	bbox.Center = { position.x, position.y, 0.0f };
	bbox.Extents = { size.x / 2, size.y / 2, 0.0f };
	return bbox;
}


Bar::Bar(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path) : GameObject(pos, sz, col, path)
{
	// 頂点データ
	vertices = CreateRectangleVertices(pos, sz, col);

	// 頂点バッファー
	auto vertSize = sizeof(Vertex) * vertices.size();
	auto vertStride = sizeof(Vertex);
	auto vertPtr = vertices.data();
	auto pVB = new VertexBuffer(vertSize, vertStride, vertPtr);
	if (!pVB->IsValid())
	{
		printf("頂点バッファの生成に失敗\n");
	}
	vertexBuffer = pVB;


	// インデックスデータ
	indices = { 0, 1, 2, 1, 2, 3 };

	// インデックスバッファー
	auto indSize = sizeof(uint32_t) * indices.size();
	auto indPtr = indices.data();
	auto pIB = new IndexBuffer(indSize, indPtr);
	if (!pIB->IsValid())
	{
		printf("インデックスバッファの生成に失敗\n");
	}
	indexBuffer = pIB;
}

Bar::~Bar() {
	delete vertexBuffer;
	delete indexBuffer;
}

void Bar::Update(float mouseInputX, float mouseInputY) 
{
	position.x = mouseInputX * 2.8;
	auto ptr = constantBuffer[g_Engine->CurrentBackBufferIndex()]->GetPtr<Transform>();
	ptr->World = XMMatrixTranslation(position.x - offset.x, position.y - offset.y, 0.0f);
}


Block::Block(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path) : GameObject(pos, sz, col, path)
{
	// 頂点データ
	vertices = CreateRectangleVertices(pos, sz, col);

	// 頂点バッファー
	auto vertSize = sizeof(Vertex) * vertices.size();
	auto vertStride = sizeof(Vertex);
	auto vertPtr = vertices.data();
	auto pVB = new VertexBuffer(vertSize, vertStride, vertPtr);
	if (!pVB->IsValid())
	{
		printf("頂点バッファの生成に失敗\n");
	}
	vertexBuffer = pVB;


	// インデックスデータ
	indices = { 0, 1, 2, 1, 2, 3 };

	// インデックスバッファー
	auto indSize = sizeof(uint32_t) * indices.size();
	auto indPtr = indices.data();
	auto pIB = new IndexBuffer(indSize, indPtr);
	if (!pIB->IsValid())
	{
		printf("インデックスバッファの生成に失敗\n");
	}
	indexBuffer = pIB;
}

Block::~Block() 
{
	delete vertexBuffer;
	delete indexBuffer;
}

Ball::Ball(XMFLOAT2 pos, XMFLOAT2 sz, XMFLOAT4 col, const wchar_t* path, XMFLOAT2 vel)
	: GameObject(pos, sz, col, path), velocity(vel) 
{

	int segments =  64;
	// 頂点データ
	vertices = CreateCircleVertices(pos, sz, col, segments);

	// 頂点バッファー
	auto vertSize = sizeof(Vertex) * vertices.size();
	auto vertStride = sizeof(Vertex);
	auto vertPtr = vertices.data();
	auto pVB = new VertexBuffer(vertSize, vertStride, vertPtr);
	if (!pVB->IsValid())
	{
		printf("頂点バッファの生成に失敗\n");
	}
	vertexBuffer = pVB;


	// インデックスデータ
	indices = CreateCircleIndices(segments);

	// インデックスバッファー
	auto indSize = sizeof(uint32_t) * indices.size();
	auto indPtr = indices.data();
	auto pIB = new IndexBuffer(indSize, indPtr);
	if (!pIB->IsValid())
	{
		printf("インデックスバッファの生成に失敗\n");
	}
	indexBuffer = pIB;
} 

Ball::~Ball() 
{
	delete vertexBuffer;
	delete indexBuffer;
}

void Ball::Update(float deltaTime) 
{
	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;

	auto ptr = constantBuffer[g_Engine->CurrentBackBufferIndex()]->GetPtr<Transform>();
	ptr->World = XMMatrixTranslation(position.x - offset.x, position.y - offset.y, 0.0f);
}

void Ball::Reflect(bool reflectX, bool reflectY) 
{
	
	if (reflectX) {
		velocity.x = -velocity.x;
	}

	if (reflectY) {
		velocity.y = -velocity.y;
	}
}

XMFLOAT2 Ball::getVelocity() 
{
	return velocity;
}

void Ball::setVelocity(XMFLOAT2 newVel) 
{
	velocity = newVel;
}
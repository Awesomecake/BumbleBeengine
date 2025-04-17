#include "Systems.h"

#pragma region TransformLogic
DirectX::XMFLOAT4X4	Systems::CalcWorldMatrix(TransformComponent& transform_comp)
{
	if (transform_comp.IsDirty())
	{
		//Get Values
		DirectX::XMFLOAT3 pos = transform_comp.GetPosition();
		DirectX::XMFLOAT3 scale = transform_comp.GetScale();
		DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();

		//Calculate Matrices
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
			DirectX::XMMatrixRotationQuaternion(quat) * 
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

		XMStoreFloat4x4(&transform_comp.worldMatrix, world);
		XMStoreFloat4x4(&transform_comp.worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		transform_comp.SetIsDirty(false);
	}

	return transform_comp.worldMatrix;
}

DirectX::XMFLOAT4X4 Systems::CalcWorldInverseTransposeMatrix(TransformComponent& transform_comp)
{
	if (transform_comp.IsDirty())
	{
		//Get Values
		DirectX::XMFLOAT3 pos = transform_comp.GetPosition();
		DirectX::XMFLOAT3 scale = transform_comp.GetScale();
		DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();

		//Calculate Matrices
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
			DirectX::XMMatrixRotationQuaternion(quat) * 
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		
		XMStoreFloat4x4(&transform_comp.worldMatrix, world);
		XMStoreFloat4x4(&transform_comp.worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		transform_comp.SetIsDirty(false);
	}

	return transform_comp.worldInverseTransposeMatrix;
}

DirectX::XMFLOAT3 Systems::GetRight(TransformComponent& transform_comp)
{
	DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

	DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(1, 0, 0, 0);
	rightVector = DirectX::XMVector3Rotate(rightVector, quat);

	DirectX::XMFLOAT3 newRightVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newRightVec, rightVector);
	return newRightVec;
}

DirectX::XMFLOAT3 Systems::GetUp(TransformComponent& transform_comp)
{
	DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0, 1, 0, 0);
	upVector = DirectX::XMVector3Rotate(upVector, quat);

	DirectX::XMFLOAT3 newUpVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newUpVec, upVector);
	return newUpVec;
}

DirectX::XMFLOAT3 Systems::GetForward(TransformComponent& transform_comp)
{
	DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);

	DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0, 0, 1, 0);
	forwardVector = DirectX::XMVector3Rotate(forwardVector,quat);

	DirectX::XMFLOAT3 newForwardVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newForwardVec, forwardVector);
	return newForwardVec;
}
#pragma endregion

#pragma region PhysicsLogic
void Systems::UpdateTransformFromPhysicsBody(PhysicsManager* physicsManager, PhysicsComponent& physics_comp, TransformComponent& transform_comp)
{
	RVec3 position = physicsManager->body_interface->GetCenterOfMassPosition(physics_comp.bodyID);
	JPH::Quat rotation = physicsManager->body_interface->GetRotation(physics_comp.bodyID);

	transform_comp.SetPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
	transform_comp.SetQuaternion(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());
}
#pragma endregion

#pragma region SkyboxLogic
void Systems::DrawSkyBox(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, SkyBoxComponent skybox_comp)
{
	context->RSSetState(skybox_comp.GetRasterizerState().Get());
	context->OMSetDepthStencilState(skybox_comp.GetStencilState().Get(), 0);

	std::shared_ptr<SimplePixelShader> ps = skybox_comp.GetPixelShader();
	std::shared_ptr<SimpleVertexShader> vs = skybox_comp.GetVertexShader();

	vs->SetShader();
	vs->SetMatrix4x4("view", camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	vs->CopyAllBufferData();

	ps->SetShader();
	ps->SetSamplerState("SkyBoxSampler", skybox_comp.GetSampleState());
	ps->SetShaderResourceView("cubeMap", skybox_comp.GetCubeMapTexture());
	ps->SetFloat3("ambient", skybox_comp.ambient);
	ps->CopyAllBufferData();

	skybox_comp.GetMesh()->Draw(context);

	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(0, 0);
}

// --------------------------------------------------------
	// Loads six individual textures (the six faces of a cube map), then
	// creates a blank cube map and copies each of the six textures to
	// another face.  Afterwards, creates a shader resource view for
	// the cube map and cleans up all of the temporary resources.
	// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Systems::CreateCubeMap(
	Microsoft::WRL::ComPtr<ID3D11Device> device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not SHADER RESOURCE VIEWS!
	// - Explicitly NOT generating mipmaps, as we don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textures[6] = {};

	DirectX::CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)textures[0].GetAddressOf(), 0);
	DirectX::CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)textures[1].GetAddressOf(), 0);
	DirectX::CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)textures[2].GetAddressOf(), 0);
	DirectX::CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)textures[3].GetAddressOf(), 0);
	DirectX::CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)textures[4].GetAddressOf(), 0);
	DirectX::CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)textures[5].GetAddressOf(), 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first texture
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array" with the TEXTURECUBE flag set.  
	// This is a special GPU resource format, NOT just a 
	// C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6;            // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0;       // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;   // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1;            // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // This should be treated as a CUBE, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the final texture resource to hold the cube map
	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeMapTexture;
	device->CreateTexture2D(&cubeDesc, 0, cubeMapTexture.GetAddressOf());

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,  // Which mip (zero, since there's only one)
			i,  // Which array element?
			1); // How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture.Get(),  // Destination resource
			subresource,           // Dest subresource index (one of the array elements)
			0, 0, 0,               // XYZ location of copy
			textures[i].Get(),     // Source resource
			0,                     // Source subresource index (we're assuming there's only one)
			0);                    // Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format;         // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;        // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0;  // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture.Get(), &srvDesc, cubeSRV.GetAddressOf());

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}
#pragma endregion
#version 330 core   
layout (location = 0) in vec3 aPos;  
layout (location = 1) in vec3 aNormal;//顶点法线

out vec3 LightingColor; // 灯光照明计算之后的颜色，传递给片元着色器

uniform vec3 lightColor;//灯光颜色
uniform vec3 lightPos;//灯光的位置
uniform vec3 viewPos;//摄像机的位置

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){  

	//通过把顶点位置属性乘以模型矩阵（不是观察和投影矩阵）来把它变换到世界空间坐标
	vec3  FragPos = vec3(model * vec4(aPos, 1.0));
	//Normal = aNormal;由于我们在model矩阵中做了旋转或缩放，此时的法线却还是原来的数据，将会产生以外的效果
	vec3  Normal = mat3(transpose(inverse(model))) * aNormal;	//使用inverse和transpose函数这个法线矩阵
	gl_Position = projection * view * vec4(FragPos, 1.0); 
	

	//环境光照
    float ambientStrength = 0.1f;//环境光照强度
    vec3 ambient = ambientStrength * lightColor;//环境光

    //漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//片元->灯光
    float diff = max(dot(norm, lightDir), 0.0);//漫反射量
    vec3 diffuse = diff * lightColor;

    //镜面光照
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);//片元->摄像机
    vec3 reflectDir = reflect(-lightDir, norm);//reflect函数要求第一个向量是从光源指向片段位置的向量，返回一个根据法线对称的一个反射向量
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);//镜面光照：32是高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小。
    vec3 specular = specularStrength * spec * lightColor;

    LightingColor = ambient + diffuse + specular;
	
}	
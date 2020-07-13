#include "stdafx.h"

#include "ImGuiUtilites.h"

static size_t ImFormatString(char* buf, size_t buf_size, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int w = vsnprintf(buf, buf_size, fmt, args);
	va_end(args);
	buf[buf_size - 1] = 0;
	return (w == -1) ? buf_size : (size_t)w;
}

void ImGui::Image(Ref<Texture> texture, const ImVec2& size, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImTextureID my_tex_id = (void*)texture->GetRendererID();
	ImGui::Image(my_tex_id, size, ImVec2(0, 1), ImVec2(1, 0), tint_col, border_col);
}

IMGUI_API bool ImGui::ImageButton(Ref<Texture> texture, const ImVec2& size, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
	ImTextureID my_tex_id = (void*)texture->GetRendererID();
	return ImGui::ImageButton(my_tex_id, size, ImVec2(0, 1), ImVec2(1, 0), frame_padding, bg_col, tint_col);
}

bool ImGui::SaveStyle(const char* filename, const ImGuiStyle& style)
{
	std::filesystem::path filepath(filename);
	if (filepath.empty()) return false;
	if (!std::filesystem::exists(filepath.remove_filename()))
	{
		CLIENT_INFO("Creating Directory {0}", filepath);
		std::filesystem::create_directory(filepath.remove_filename());
	}

	std::mutex mutex;
	std::lock_guard lock(mutex);

	// Write .style file
	std::ofstream file;
	file.open(filename);
	if (!file.is_open()) return false;
	file << std::setprecision(3) << std::fixed;
	file << "[Alpha]\n" << style.Alpha << std::endl;
	file << "[WindowPadding]\n" << style.WindowPadding.x << " " << style.WindowPadding.y << std::endl;
	file << "[WindowMinSize]\n" << style.WindowMinSize.x << " " << style.WindowMinSize.y << std::endl;
	file << "[FramePadding]\n" << style.FramePadding.x << " " << style.FramePadding.y << std::endl;
	file << "[FrameRounding]\n" << style.FrameRounding << std::endl;
	file << "[ItemSpacing]\n" << style.ItemSpacing.x << " " << style.ItemSpacing.y << std::endl;
	file << "[ItemInnerSpacing]\n" << style.ItemInnerSpacing.x << " " << style.ItemInnerSpacing.y << std::endl;
	file << "[WindowRounding] \n" << style.WindowRounding << std::endl;
	file << "[ColumnsMinSpacing]\n" << style.ColumnsMinSpacing << std::endl;
	file << "[ScrollBarSize]\n" << style.ScrollbarSize << std::endl;

	for (size_t i = 0; i < ImGuiCol_COUNT; i++)
	{
		const ImVec4& c = style.Colors[i];
		file << "[" << ImGui::GetStyleColorName(i) << "]" << std::endl;
		file << c.x << " " << c.y << " " << c.z << " " << c.w << std::endl;
	}
	file << std::endl;
	file.close();

	return true;
}

bool ImGui::LoadStyle(const char* filename, ImGuiStyle& style)
{
	// Load .style file
	if (!filename)  return false;

	// Load file into memory
	FILE* f;
	if ((f = fopen(filename, "rt")) == NULL) return false;
	if (fseek(f, 0, SEEK_END)) {
		fclose(f);
		return false;
	}
	const long f_size_signed = ftell(f);
	if (f_size_signed == -1) {
		fclose(f);
		return false;
	}
	size_t f_size = (size_t)f_size_signed;
	if (fseek(f, 0, SEEK_SET)) {
		fclose(f);
		return false;
	}
	char* f_data = (char*)ImGui::MemAlloc(f_size + 1);
	f_size = fread(f_data, 1, f_size, f); // Text conversion alter read size so let's not be fussy about return value
	fclose(f);
	if (f_size == 0) {
		ImGui::MemFree(f_data);
		return false;
	}
	f_data[f_size] = 0;

	// Parse file in memory
	char name[128]; name[0] = '\0';
	const char* buf_end = f_data + f_size;
	for (const char* line_start = f_data; line_start < buf_end; )
	{
		const char* line_end = line_start;
		while (line_end < buf_end && *line_end != '\n' && *line_end != '\r')
			line_end++;

		if (name[0] == '\0' && line_start[0] == '[' && line_end > line_start && line_end[-1] == ']')
		{
			ImFormatString(name, IM_ARRAYSIZE(name), "%.*s", line_end - line_start - 2, line_start + 1);
			//fprintf(stderr,"name: %s\n",name);  // dbg
		}
		else if (name[0] != '\0')
		{

			float* pf[4] = { 0,0,0,0 };
			int npf = 0;

			// parsing 'name' here by filling the fields above
			{
				if (strcmp(name, "Alpha") == 0) { npf = 1; pf[0] = &style.Alpha; }
				else if (strcmp(name, "WindowPadding") == 0) { npf = 2; pf[0] = &style.WindowPadding.x; pf[1] = &style.WindowPadding.y; }
				else if (strcmp(name, "WindowMinSize") == 0) { npf = 2; pf[0] = &style.WindowMinSize.x; pf[1] = &style.WindowMinSize.y; }
				else if (strcmp(name, "FramePadding") == 0) { npf = 2; pf[0] = &style.FramePadding.x; pf[1] = &style.FramePadding.y; }
				else if (strcmp(name, "FrameRounding") == 0) { npf = 1; pf[0] = &style.FrameRounding; }
				else if (strcmp(name, "ItemSpacing") == 0) { npf = 2; pf[0] = &style.ItemSpacing.x; pf[1] = &style.ItemSpacing.y; }
				else if (strcmp(name, "ItemInnerSpacing") == 0) { npf = 2; pf[0] = &style.ItemInnerSpacing.x; pf[1] = &style.ItemInnerSpacing.y; }
				else if (strcmp(name, "TouchExtraPadding") == 0) { npf = 2; pf[0] = &style.TouchExtraPadding.x; pf[1] = &style.TouchExtraPadding.y; }
				else if (strcmp(name, "WindowRounding") == 0) { npf = 1; pf[0] = &style.WindowRounding; }
				else if (strcmp(name, "ColumnsMinSpacing") == 0) { npf = 1; pf[0] = &style.ColumnsMinSpacing; }
				else if (strcmp(name, "ScrollBarSize") == 0) { npf = 1; pf[0] = &style.ScrollbarSize; }
				// all the colors here
				else {
					for (int j = 0; j < ImGuiCol_COUNT; j++) {
						if (strcmp(name, ImGui::GetStyleColorName(j)) == 0) {
							npf = 4;
							ImVec4& color = style.Colors[j];
							pf[0] = &color.x; pf[1] = &color.y; pf[2] = &color.z; pf[3] = &color.w;
							break;
						}
					}
				}
			}

			//fprintf(stderr,"name: %s npf=%d\n",name,npf);  // dbg
			// parsing values here and filling pf[]
			float x, y, z, w;
			switch (npf) {
			case 1:
				if (sscanf(line_start, "%f", &x) == npf) {
					*pf[0] = x;
				}
				else ENGINE_WARN("ImGui::LoadStyle({0}): skipped [{1}] (parsing error)", filename, name);
				break;
			case 2:
				if (sscanf(line_start, "%f %f", &x, &y) == npf) {
					*pf[0] = x; *pf[1] = y;
				}
				else ENGINE_WARN("ImGui::LoadStyle({0}): skipped [{1}] (parsing error)", filename, name);
				break;
			case 3:
				if (sscanf(line_start, "%f %f %f", &x, &y, &z) == npf) {
					*pf[0] = x; *pf[1] = y; *pf[2] = z;
				}
				else ENGINE_WARN("ImGui::LoadStyle({0}): skipped [{1}] (parsing error)", filename, name);
				break;
			case 4:
				if (sscanf(line_start, "%f %f %f %f", &x, &y, &z, &w) == npf) {
					*pf[0] = x; *pf[1] = y; *pf[2] = z; *pf[3] = w;
				}
				else ENGINE_WARN("ImGui::LoadStyle({0}): skipped [{1}] (parsing error)", filename, name);
				break;
			default:
				ENGINE_WARN("ImGui::LoadStyle({0}): skipped [{1}] (unknown field)", filename, name);
				break;
			}
			/*
			// Same reference code from <imgui.cpp> to help parsing
			float x, y;
			int i;
			if (sscanf(line_start, "Pos=%f,%f", &x, &y) == 2)
				settings->Pos = ImVec2(x, y);
			else if (sscanf(line_start, "Size=%f,%f", &x, &y) == 2)
				settings->Size = ImMax(ImVec2(x, y), g.Style.WindowMinSize);
			else if (sscanf(line_start, "Collapsed=%d", &i) == 1)
				settings->Collapsed = (i != 0);
			*/
			//---------------------------------------------------------------------------------    
			name[0] = '\0'; // mandatory
		}

		line_start = line_end + 1;
	}

	// Release memory
	ImGui::MemFree(f_data);
	return true;
}

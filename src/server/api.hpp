#ifndef ARIS_SERVER_API_H_
#define ARIS_SERVER_API_H_

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <tuple>

#include "aris.hpp"

namespace aris::server
{
	class Calculator
	{
	public:
		using BuiltInFunction = std::function<std::any(std::vector<std::any>&)>;
		using BinaryOperatorFunction = std::function<std::any(std::any&, std::any&)>;
		using UnaryOperatorFunction = std::function<std::any(std::any&)>;

		auto calculateExpression(std::string_view expression) const->std::pair<std::string, std::any>;

		auto addTypename(std::string_view tpn)->void;
		auto addOperator(std::string_view opr, int ul_priority, int ur_priority, int b_priority)->void;
		auto addVariable(std::string_view var, std::string_view type, const std::any &value)->void;
		auto addFunction(std::string_view fun, const std::vector<std::string> &param_type, std::string_view ret_type, BuiltInFunction f)->void;

		auto addUnaryLeftOperatorFunction(std::string_view opr, std::string_view p_type, std::string_view ret_type, UnaryOperatorFunction f)->void;
		auto addUnaryRightOperatorFunction(std::string_view opr, std::string_view p_type, std::string_view ret_type, UnaryOperatorFunction f)->void;
		auto addBinaryOperatorFunction(std::string_view opr, std::string_view p1_type, std::string_view p2_type, std::string_view ret_type, BinaryOperatorFunction f)->void;
		auto clearVariables()->void;

		virtual ~Calculator();
		explicit Calculator(const std::string &name = "calculator");
		ARIS_DECLARE_BIG_FOUR(Calculator);

	private:
		struct Imp;
		aris::core::ImpPtr<Imp> imp_;
	};
	
	class MakeBlockly
	{
	public:
		auto parse_raw(std::string_view cmd_str)->std::tuple<std::string_view, std::map<std::string_view, std::string_view>>
		{
			auto cut_str = [](std::string_view &input, const char *c)->std::string_view
			{
				// 此时c中字符是或的关系 //
				auto point = input.find_first_of(c);
				auto ret = input.substr(0, point);
				input = point == std::string::npos ? std::string_view() : input.substr(point);
				return ret;
			};
			auto trim_left = [](std::string_view input)->std::string_view
			{
				auto point = input.find_first_not_of(' ');
				return point == std::string::npos ? std::string_view() : input.substr(point, std::string::npos);
			};
			auto get_param_value = [&](std::string_view &cmd_str)->std::string_view
			{
				int brace_num = 0;
				int i = 1;
				for (i = 1; i < cmd_str.size() && !(std::isspace(cmd_str[i]) && brace_num == 0); ++i)
				{
					switch (cmd_str[i])
					{
					case '{':
						++brace_num;
						break;
					case '}':
						--brace_num;
						if (brace_num < 0)THROW_FILE_LINE("brace not pair");
						break;
					default:
						break;
					}
				}

				if (brace_num)THROW_FILE_LINE("brace not pair");

				auto ret = cmd_str.substr(1, i - 1);// here is length
				cmd_str = trim_left(cmd_str.substr(i));
				return ret;
			};

			std::string_view cmd;
			std::map<std::string_view, std::string_view> param_map;

			if (cmd = cut_str(cmd_str, " "); cmd.empty())THROW_FILE_LINE("invalid command string: please at least contain a word");
			cmd_str = trim_left(cmd_str);

			for (; !cmd_str.empty();)
			{
				auto param_name_origin = cut_str(cmd_str, " =");
				cmd_str = trim_left(cmd_str);

				auto param_name = param_name_origin.substr(2);
				auto param_value = get_param_value(cmd_str);

				param_map.insert(make_pair(std::string_view(param_name), std::string_view(param_value)));
			}
			return std::make_tuple(cmd, param_map);
		}
		
		auto make(std::filesystem::path program, std::filesystem::path blockly_dir)
		{
			if (!std::filesystem::exists(blockly_dir))
			{
				std::filesystem::create_directories(blockly_dir);
			}
			cal.clearVariables();
			//////////////////////////make var/////////////////////////////////
			{
				std::fstream pf(program);
				aris::core::XmlDocument var_doc;
				var_doc.InsertEndChild(var_doc.NewElement("xml"));
				auto lbv = var_doc.RootElement();

				std::string line;
				bool is_first = true;
				while (std::getline(pf, line, '\n'))
				{
					std::cout << line << std::endl;

					int id = std::stoi(line.substr(0, line.find_first_of(":")));
					auto data = line.substr(line.find_first_of(":") + 1);
					std::stringstream ss(data);
					std::string word;
					ss >> word;

					if (word == "var")
					{
						std::string type;
						std::string name;
						ss >> type;
						ss >> name;

						cal.addVariable(name, type, name);
					}
					
				}

				auto v = (blockly_dir / ("test.dat"));
				var_doc.SaveFile(v.string().c_str(), false);
			}

			//////////////////////////make pro/////////////////////////////////
			{
				std::fstream pf(program);
				aris::core::XmlDocument pro_doc;
				pro_doc.InsertEndChild(pro_doc.NewElement("xml"));
				auto lbp = pro_doc.RootElement();

				std::string line;
				std::vector<aris::core::XmlElement*> stack;

				bool is_first = true;
				while (std::getline(pf, line, '\n'))
				{
					int id = std::stoi(line.substr(0, line.find_first_of(":")));
					auto data = line.substr(line.find_first_of(":") + 1);
					std::stringstream ss(data);
					std::string word;
					ss >> word;

					if (word == "main" || word == "function" || word == "if" || word == "while")
					{
						auto nbp = pro_doc.NewElement("block");
						nbp->SetAttribute("type", word.c_str());
						nbp->SetAttribute("id", id);
						stack.push_back(nbp);
						if (is_first)
						{
							nbp->SetAttribute("x", 30);
							nbp->SetAttribute("y", 30);
							is_first = false;
							lbp->InsertEndChild(nbp);
						}
						else
						{
							if (lbp->Name() == std::string("statement"))
							{
								lbp->InsertEndChild(nbp);
							}
							else
							{
								auto next = pro_doc.NewElement("next");
								lbp->InsertEndChild(next);
								next->InsertEndChild(nbp);
							}
						}

						auto state = pro_doc.NewElement("statement");
						state->SetAttribute("name", "body");
						nbp->InsertEndChild(state);

						lbp = state;

						if (word == "function")
						{
							std::string func_name;
							ss >> func_name;
							auto field = pro_doc.NewElement("field");
							field->SetAttribute("name", "func_name");
							field->SetText(func_name.c_str());
							nbp->InsertEndChild(field);
						}

						if (word == "if")
						{
							this->current_doc_ = &pro_doc;

							auto condition = pro_doc.NewElement("value");
							condition->SetAttribute("name", "condition");

							auto[name, value] = this->cal.calculateExpression(data.substr(word.size()));
							auto v = std::any_cast<aris::core::XmlElement*>(value);
							condition->InsertFirstChild(std::any_cast<aris::core::XmlElement*>(value));
						

							nbp->InsertFirstChild(condition);


						}
					}
					else if (word == "endmain" || word == "endfunction" || word == "endif" || word == "endwhile")
					{
						lbp = stack.back();
						stack.pop_back();
					}
					else if (word == "var" || word == "set")
					{
					}
					else
					{
						auto[cmd, params] = parse_raw(data);

						auto nbp = pro_doc.NewElement("block");
						nbp->SetAttribute("type", std::string(cmd).c_str());
						nbp->SetAttribute("id", id);
						for (auto &param : params)
						{
							auto field = pro_doc.NewElement("field");
							field->SetAttribute("name", std::string(param.first).c_str());
							field->SetText(std::string(param.second).c_str());
							nbp->InsertEndChild(field);
						}

						if (is_first)
						{
							nbp->SetAttribute("x", 30);
							nbp->SetAttribute("y", 30);
							is_first = false;
							lbp->InsertEndChild(nbp);
						}
						else
						{
							if (lbp->Name() == std::string("statement"))
							{
								lbp->InsertEndChild(nbp);
							}
							else
							{
								auto next = pro_doc.NewElement("next");
								lbp->InsertEndChild(next);
								next->InsertEndChild(nbp);
							}
						}

						lbp = nbp;
					}
				}

				auto p = (blockly_dir / ("test.pro"));
				pro_doc.SaveFile(p.string().c_str(), false);
				
			}
		}

		MakeBlockly() 
		{
			auto make_num_block = [&](aris::core::XmlDocument* doc, double number)->aris::core::XmlElement* 
			{
				auto block = doc->NewElement("block");
				block->SetAttribute("type", "number");

				auto field = doc->NewElement("field");
				field->SetAttribute("name", "number");
				field->SetText(number);
				block->InsertEndChild(field);

				return block;
			};
			
			cal.addOperator("=", 0, 0, 1);

			cal.addTypename("Number");
			cal.addTypename("Block");

			cal.addOperator("+", 10, 0, 10);
			cal.addBinaryOperatorFunction("+", "Number", "Number", "Block", [&](std::any& p1, std::any&p2)->std::any
			{
				auto doc = this->current_doc_;

				auto block = doc->NewElement("block");
				block->SetAttribute("type", "math_opr");

				auto field = doc->NewElement("field");
				field->SetAttribute("name", "opr");
				field->SetText("+");
				block->InsertEndChild(field);

				auto left = doc->NewElement("value");
				left->SetAttribute("name", "left");
				left->InsertEndChild(make_num_block(doc, std::any_cast<double>(p1)));
				block->InsertEndChild(left);


				auto right = doc->NewElement("value");
				right->SetAttribute("name", "right");
				right->InsertEndChild(make_num_block(doc, std::any_cast<double>(p2)));
				block->InsertEndChild(right);

				return block;
			});
			cal.addBinaryOperatorFunction("+", "Number", "Block", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<double>(p1) + std::any_cast<aris::core::Matrix&>(p2);
			});
			cal.addBinaryOperatorFunction("+", "Block", "Number", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<aris::core::Matrix&>(p1) + std::any_cast<double>(p2);
			});
			cal.addBinaryOperatorFunction("+", "Block", "Block", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<aris::core::Matrix&>(p1) + std::any_cast<aris::core::Matrix>(p2);
			});

			cal.addOperator("-", 10, 0, 10);
			cal.addUnaryLeftOperatorFunction("-", "Number", "Number", [](std::any& p1)->std::any {return -std::any_cast<double>(p1); });
			cal.addBinaryOperatorFunction("-", "Number", "Number", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<double>(p1) - std::any_cast<double>(p2);
			});
			cal.addBinaryOperatorFunction("-", "Number", "Block", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<double>(p1) - std::any_cast<aris::core::Matrix&>(p2);
			});
			cal.addBinaryOperatorFunction("-", "Block", "Number", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<aris::core::Matrix&>(p1) - std::any_cast<double>(p2);
			});
			cal.addBinaryOperatorFunction("-", "Block", "Block", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<aris::core::Matrix&>(p1) - std::any_cast<aris::core::Matrix>(p2);
			});

			cal.addOperator("*", 0, 0, 20);
			cal.addBinaryOperatorFunction("*", "Number", "Number", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<double>(p1) * std::any_cast<double>(p2);
			});
			cal.addBinaryOperatorFunction("*", "Number", "Block", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<double>(p1) * std::any_cast<aris::core::Matrix&>(p2);
			});
			cal.addBinaryOperatorFunction("*", "Block", "Number", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<aris::core::Matrix&>(p1) * std::any_cast<double>(p2);
			});
			cal.addBinaryOperatorFunction("*", "Block", "Block", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<aris::core::Matrix&>(p1) * std::any_cast<aris::core::Matrix>(p2);
			});

			cal.addOperator("/", 0, 0, 20);
			cal.addBinaryOperatorFunction("/", "Number", "Number", "Block", [](std::any& p1, std::any&p2)->std::any
			{
				return std::any_cast<double>(p1) / std::any_cast<double>(p2);
			});
		}

		MakeBlockly::MakeBlockly(const MakeBlockly&) = delete;
		MakeBlockly &operator =(const MakeBlockly&) = delete;

		Calculator cal;
		aris::core::XmlDocument *current_doc_;
	};
	
	
	
	
	auto setRootPath(std::filesystem::path path)->void;

	auto fetchInterfaceConfig()->std::string;
	auto updateDashboard(std::string dash_id, std::string js_str)->std::string;
	auto createCell(std::string dash_id, std::string cell)->std::string;
	auto deleteCell(std::string dash_id, std::string cell_id)->std::string;

	auto fetchPrograms()->std::string;
	auto createProgram(std::string pro_name)->std::string;
	auto updateProgram(std::string js_str, std::string data)->std::string;
	auto deleteProgram(std::string pro_name)->std::string;
	auto renameProgram(std::string old_name, std::string new_name_js)->std::string;
}

#endif
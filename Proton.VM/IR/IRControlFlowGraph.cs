using Proton.VM.IR.Instructions;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace Proton.VM.IR
{
	public sealed class IRControlFlowGraph
	{
		public List<IRControlFlowGraphNode> Nodes = new List<IRControlFlowGraphNode>();

		public static IRControlFlowGraph Build(IRMethod pMethod)
		{
			if (pMethod.Instructions.Count == 0) return null;

			HashSet<IRInstruction> sourceNodeBreaks = new HashSet<IRInstruction>();
			HashSet<IRInstruction> destinationNodeBreaks = new HashSet<IRInstruction>();
			int lIRIdx = pMethod.Instructions.Count - 1;
			foreach (IRInstruction instruction in pMethod.Instructions)
			{
				bool lastInstruction = instruction.IRIndex == lIRIdx;
				switch (instruction.Opcode)
				{
					case IROpcode.Branch:
						{
							IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
							sourceNodeBreaks.Add(instruction);
							destinationNodeBreaks.Add(branchInstruction.TargetIRInstruction);
							break;
						}
					case IROpcode.Switch:
						{
							IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
							sourceNodeBreaks.Add(instruction);
							switchInstruction.TargetIRInstructions.ForEach(i => destinationNodeBreaks.Add(i));
							break;
						}
					case IROpcode.Leave:
						{
							IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
							sourceNodeBreaks.Add(instruction);
							destinationNodeBreaks.Add(leaveInstruction.TargetIRInstruction);
							break;
						}
					case IROpcode.EndFinally:
						{
							IREndFinallyInstruction endFinallyInstruction = (IREndFinallyInstruction)instruction;
							sourceNodeBreaks.Add(instruction);
							if (!lastInstruction) destinationNodeBreaks.Add(pMethod.Instructions[instruction.IRIndex + 1]);
							break;
						}
					case IROpcode.Throw:
					case IROpcode.Rethrow:
					case IROpcode.Return:
						sourceNodeBreaks.Add(instruction);
						break;
					default: break;
				}
			}

			IRControlFlowGraph cfg = new IRControlFlowGraph();
			IRControlFlowGraphNode currentNode = new IRControlFlowGraphNode(0);
			cfg.Nodes.Add(currentNode);
			foreach (IRInstruction instruction in pMethod.Instructions)
			{
				bool lastInstruction = instruction.IRIndex == lIRIdx;
				bool startFromSource = sourceNodeBreaks.Contains(instruction);
				bool startFromDestination = destinationNodeBreaks.Contains(instruction);
				if (startFromSource && startFromDestination)
				{
					if (currentNode.Instructions.Count > 0)
					{
						currentNode = new IRControlFlowGraphNode(cfg.Nodes.Count);
						cfg.Nodes.Add(currentNode);
					}
					currentNode.Instructions.Add(instruction);
					if (!lastInstruction)
					{
						currentNode = new IRControlFlowGraphNode(cfg.Nodes.Count);
						cfg.Nodes.Add(currentNode);
					}
				}
				else if (startFromSource)
				{
					currentNode.Instructions.Add(instruction);
					if (!lastInstruction)
					{
						currentNode = new IRControlFlowGraphNode(cfg.Nodes.Count);
						cfg.Nodes.Add(currentNode);
					}
				}
				else if (startFromDestination)
				{
					if (currentNode.Instructions.Count > 0)
					{
						currentNode = new IRControlFlowGraphNode(cfg.Nodes.Count);
						cfg.Nodes.Add(currentNode);
					}
					currentNode.Instructions.Add(instruction);
				}
				else currentNode.Instructions.Add(instruction);
			}

			foreach (IRControlFlowGraphNode node in cfg.Nodes)
			{
				IRInstruction instruction = node.Instructions[node.Instructions.Count - 1];
				switch (instruction.Opcode)
				{
					case IROpcode.Branch:
						{
							IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
							IRControlFlowGraphNode childNode = cfg.Nodes.Find(n => n.Instructions[0] == branchInstruction.TargetIRInstruction);
							if (childNode == null) throw new NullReferenceException();
							if (branchInstruction.BranchCondition != IRBranchCondition.Always) node.LinkTo(cfg.Nodes[node.Index + 1]);
							node.LinkTo(childNode);
							break;
						}
					case IROpcode.Switch:
						{
							IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
							node.LinkTo(cfg.Nodes[node.Index + 1]);
							foreach (IRInstruction targetInstruction in switchInstruction.TargetIRInstructions)
							{
								IRControlFlowGraphNode childNode = cfg.Nodes.Find(n => n.Instructions[0] == targetInstruction);
								if (childNode == null) throw new NullReferenceException();
								node.LinkTo(childNode);
							}
							break;
						}
					case IROpcode.Leave:
						{
							IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
							IRControlFlowGraphNode childNode = cfg.Nodes.Find(n => n.Instructions[0] == leaveInstruction.TargetIRInstruction);
							if (childNode == null) throw new NullReferenceException();
							node.LinkTo(childNode);
							break;
						}
					case IROpcode.Throw:
					case IROpcode.Rethrow:
					case IROpcode.Return: continue;
					default: if (cfg.Nodes.Count > node.Index + 1) node.LinkTo(cfg.Nodes[node.Index + 1]); break;
				}
			}

			List<IRControlFlowGraphNode> allDeadNodes = new List<IRControlFlowGraphNode>(32);
			List<IRControlFlowGraphNode> deadNodes = null;
			while ((deadNodes = cfg.Nodes.FindAll(n => n.Index > 0 && n.ParentNodes.Count == 0)).Count > 0)
			{
				allDeadNodes.AddRange(deadNodes);
				foreach (IRControlFlowGraphNode deadNode in deadNodes)
				{
					foreach (IRControlFlowGraphNode childNode in deadNode.ChildNodes) childNode.ParentNodes.Remove(deadNode);
					cfg.Nodes.RemoveAt(deadNode.Index);
					for (int nodeIndex = deadNode.Index; nodeIndex < cfg.Nodes.Count; ++nodeIndex)
					{
						IRControlFlowGraphNode node = cfg.Nodes[nodeIndex];
						node.Index -= 1;
					}
				}
			}

			cfg.Nodes.ForEach(n => n.Dominators = new BitVector(cfg.Nodes.Count, true));
			BitVector intersectedParentDominators = new BitVector(cfg.Nodes.Count);
			HashSet<IRControlFlowGraphNode> todoSet = new HashSet<IRControlFlowGraphNode>();
			HashSet<IRControlFlowGraphNode>.Enumerator todoSetEnumerator;
			todoSet.Add(cfg.Nodes[0]);
			while (todoSet.Count > 0)
			{
				todoSetEnumerator = todoSet.GetEnumerator();
				todoSetEnumerator.MoveNext();
				IRControlFlowGraphNode node = todoSetEnumerator.Current;
				todoSet.Remove(node);
				intersectedParentDominators.SetAll(node.ParentNodes.Count > 0);
				node.ParentNodes.ForEach(n => intersectedParentDominators.AndEquals(n.Dominators));
				intersectedParentDominators.Set(node.Index, true);
				if (!intersectedParentDominators.Equals(node.Dominators))
				{
					node.Dominators = new BitVector(intersectedParentDominators);
					node.ChildNodes.ForEach(n => todoSet.Add(n));
				}
			}
			foreach (IRControlFlowGraphNode node in cfg.Nodes)
			{
				for (int index = 0; index < node.Dominators.Count; ++index)
				{
					if (node.Dominators.Get(index))
						node.DominatorsCount++;
				}
			}
			foreach (IRControlFlowGraphNode node in cfg.Nodes)
			{
				int max = -1;
				foreach (IRControlFlowGraphNode innerNode in cfg.Nodes)
				{
					if (node.Dominators.Get(innerNode.Index) && node != innerNode && innerNode.DominatorsCount > max)
					{
						max = innerNode.DominatorsCount;
						node.Dominator = innerNode;
					}
				}
			}
			cfg.Nodes[0].Dominator = cfg.Nodes[0];

			foreach (IRControlFlowGraphNode node in cfg.Nodes)
			{
				if (node.ParentNodes.Count < 2) continue;
				foreach (IRControlFlowGraphNode parentNode in node.ParentNodes)
				{
					IRControlFlowGraphNode treeNode = parentNode;
					while (treeNode != node.Dominator)
					{
						treeNode.Frontiers.Add(node);
						treeNode = treeNode.Dominator;
					}
				}
			}

			return cfg;
		}

		public IRControlFlowGraphNode FindInstructionNode(IRInstruction pInstruction)
		{
			foreach (IRControlFlowGraphNode node in Nodes)
			{
				foreach (IRInstruction instruction in node.Instructions)
				{
					if (pInstruction == instruction) return node;
				}
			}
			throw new NullReferenceException();
		}

		public override string ToString()
		{
			StringBuilder sb = new StringBuilder();
			Nodes.ForEach(n => sb.AppendLine(string.Format("Node #{0}, 0x{1:X} - 0x{2:X}", n.Index, n.Instructions[0].ILOffset, n.Instructions[n.Instructions.Count - 1].ILOffset)));
			return sb.ToString();
		}
	}
}

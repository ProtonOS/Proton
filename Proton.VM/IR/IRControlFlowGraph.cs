using Proton.VM.IR.Instructions;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
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
            foreach (IRInstruction instruction in pMethod.Instructions)
            {
                switch (instruction.Opcode)
                {
                    case IROpcode.Branch:
                        {
                            IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
                            if (!sourceNodeBreaks.Contains(instruction)) sourceNodeBreaks.Add(instruction);
                            if (!destinationNodeBreaks.Contains(branchInstruction.TargetIRInstruction)) destinationNodeBreaks.Add(branchInstruction.TargetIRInstruction);
                            break;
                        }
                    case IROpcode.Switch:
                        {
                            IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
                            if (!sourceNodeBreaks.Contains(instruction)) sourceNodeBreaks.Add(instruction);
                            foreach (IRInstruction targetIRInstruction in switchInstruction.TargetIRInstructions)
                            {
                                if (!destinationNodeBreaks.Contains(targetIRInstruction))
                                {
                                    destinationNodeBreaks.Add(targetIRInstruction);
                                }
                            }
                            break;
                        }
                    case IROpcode.Leave:
                        {
                            IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
                            if (!sourceNodeBreaks.Contains(instruction)) sourceNodeBreaks.Add(instruction);
                            if (!destinationNodeBreaks.Contains(leaveInstruction.TargetIRInstruction)) destinationNodeBreaks.Add(leaveInstruction.TargetIRInstruction);
                            break;
                        }
                    default: break;
                }
            }

            IRControlFlowGraph cfg = new IRControlFlowGraph();
			IRControlFlowGraphNode currentNode = new IRControlFlowGraphNode(0);
            cfg.Nodes.Add(currentNode);
            foreach (IRInstruction instruction in pMethod.Instructions)
            {
                bool lastInstruction = instruction == pMethod.Instructions[pMethod.Instructions.Count - 1];
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
					currentNode = new IRControlFlowGraphNode(cfg.Nodes.Count);
                    cfg.Nodes.Add(currentNode);
                }
                else if (startFromSource)
                {
                    currentNode.Instructions.Add(instruction);
					currentNode = new IRControlFlowGraphNode(cfg.Nodes.Count);
                    cfg.Nodes.Add(currentNode);
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
                    case IROpcode.Return: continue;
                    default: node.LinkTo(cfg.Nodes[node.Index + 1]); break;
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

			cfg.Nodes.ForEach(n => n.Dominators = new BitArray(cfg.Nodes.Count, true));
			cfg.Nodes[0].Dominators.SetAll(false);
			cfg.Nodes[0].Dominators.Set(0, true);
			HashSet<IRControlFlowGraphNode> todoSet = new HashSet<IRControlFlowGraphNode>();
			todoSet.Add(cfg.Nodes[0]);
			while (todoSet.Count > 0)
			{
				IRControlFlowGraphNode node = todoSet.ElementAt(0);
				todoSet.Remove(node);
				BitArray intersectedParentDominators = new BitArray(cfg.Nodes.Count, true);
				node.ParentNodes.ForEach(n => intersectedParentDominators = intersectedParentDominators.And(n.Dominators));
				intersectedParentDominators.Set(node.Index, true);
				bool dominatorsEqual = true;
				for (int index = 0; dominatorsEqual && index < intersectedParentDominators.Length; ++index) dominatorsEqual = intersectedParentDominators[index] == node.Dominators[index];
				if (!dominatorsEqual)
				{
					node.Dominators = intersectedParentDominators;
					node.ChildNodes.ForEach(n => { if (!todoSet.Contains(n)) todoSet.Add(n); });
				}
			}
			foreach (IRControlFlowGraphNode node in cfg.Nodes)
			{
				for (int index = 0; index < node.Dominators.Length; ++index)
				{
					if (node.Dominators[index])
						node.DominatorsCount++;
				}
			}
			foreach (IRControlFlowGraphNode node in cfg.Nodes)
			{
				int max = -1;
				foreach (IRControlFlowGraphNode innerNode in cfg.Nodes)
				{
					if (node.Dominators[innerNode.Index] && node != innerNode && innerNode.DominatorsCount > max)
					{
						max = innerNode.DominatorsCount;
						node.Dominator = innerNode;
					}
				}
			}

			//foreach (Node node in cfg.Nodes)
			//{
			//    if (node.ParentNodes.Count < 2) continue;
			//    foreach (Node parentNode in node.ParentNodes)
			//    {
			//        if (parentNode == node.Dominator)
			//        {
			//            parentNode.DestinationFrontiers.Add(node);
			//            node.SourceFrontiers.Add(parentNode);
			//        }
			//        Node treeNode = parentNode;
			//        while (treeNode != node.Dominator)
			//        {
			//            treeNode.DestinationFrontiers.Add(node);
			//            node.SourceFrontiers.Add(treeNode);
			//            treeNode = treeNode.Dominator;
			//        }
			//    }
			//}

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

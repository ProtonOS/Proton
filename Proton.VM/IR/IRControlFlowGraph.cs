using Proton.VM.IR.Instructions;
using System;
using System.Collections.Generic;
using System.Text;

namespace Proton.VM.IR
{
    public sealed class IRControlFlowGraph
    {
        public sealed class Node
        {
            public uint Index = 0;
            public List<IRInstruction> Instructions = new List<IRInstruction>();
            public List<Node> ChildNodes = new List<Node>();
            public List<Node> ParentNodes = new List<Node>();

            public Node(uint pIndex) { Index = pIndex; }

            public void LinkTo(Node pChildNode)
            {
                ChildNodes.Add(pChildNode);
                pChildNode.ParentNodes.Add(this);
            }
        }

        public List<Node> Nodes = new List<Node>();

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
            Node currentNode = new Node(0);
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
                        currentNode = new Node((uint)cfg.Nodes.Count);
                        cfg.Nodes.Add(currentNode);
                    }
                    currentNode.Instructions.Add(instruction);
                    currentNode = new Node((uint)cfg.Nodes.Count);
                    cfg.Nodes.Add(currentNode);
                }
                else if (startFromSource)
                {
                    currentNode.Instructions.Add(instruction);
                    currentNode = new Node((uint)cfg.Nodes.Count);
                    cfg.Nodes.Add(currentNode);
                }
                else if (startFromDestination)
                {
                    if (currentNode.Instructions.Count > 0)
                    {
                        currentNode = new Node((uint)cfg.Nodes.Count);
                        cfg.Nodes.Add(currentNode);
                    }
                    currentNode.Instructions.Add(instruction);
                }
                else currentNode.Instructions.Add(instruction);
            }

            for (int nodeIndex = 0; nodeIndex < cfg.Nodes.Count; ++nodeIndex)
            {
                Node node = cfg.Nodes[nodeIndex];
                IRInstruction instruction = node.Instructions[node.Instructions.Count - 1];
                switch (instruction.Opcode)
                {
                    case IROpcode.Branch:
                        {
                            IRBranchInstruction branchInstruction = (IRBranchInstruction)instruction;
                            Node childNode = cfg.Nodes.Find(n => n.Instructions[0] == branchInstruction.TargetIRInstruction);
                            if (childNode == null) throw new NullReferenceException();
                            if (branchInstruction.BranchCondition != IRBranchCondition.Always) node.LinkTo(cfg.Nodes[nodeIndex + 1]);
                            node.LinkTo(childNode);
                            break;
                        }
                    case IROpcode.Switch:
                        {
                            IRSwitchInstruction switchInstruction = (IRSwitchInstruction)instruction;
                            node.LinkTo(cfg.Nodes[nodeIndex + 1]);
                            foreach (IRInstruction targetInstruction in switchInstruction.TargetIRInstructions)
                            {
                                Node childNode = cfg.Nodes.Find(n => n.Instructions[0] == targetInstruction);
                                if (childNode == null) throw new NullReferenceException();
                                node.LinkTo(childNode);
                            }
                            break;
                        }
                    case IROpcode.Leave:
                        {
                            IRLeaveInstruction leaveInstruction = (IRLeaveInstruction)instruction;
                            Node childNode = cfg.Nodes.Find(n => n.Instructions[0] == leaveInstruction.TargetIRInstruction);
                            if (childNode == null) throw new NullReferenceException();
                            node.LinkTo(childNode);
                            break;
                        }
                    case IROpcode.Throw:
                    case IROpcode.Return: continue;
                    default: node.LinkTo(cfg.Nodes[nodeIndex + 1]); break;
                }
            }
            return cfg;
        }

        public Node FindInstructionNode(IRInstruction pInstruction)
        {
            foreach (Node node in Nodes)
            {
                foreach (IRInstruction instruction in node.Instructions)
                {
                    if (pInstruction == instruction) return node;
                }
            }
            throw new NullReferenceException();
        }

        public void PopulateParents(Node pNode, List<Node> pParents)
        {
            if (pParents.Contains(pNode)) return;
            pParents.Add(pNode);
            pNode.ParentNodes.ForEach(n => PopulateParents(n, pParents));
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            Nodes.ForEach(n => sb.AppendLine(string.Format("Node #{0}, 0x{1:X} - 0x{2:X}", n.Index, n.Instructions[0].ILOffset, n.Instructions[n.Instructions.Count - 1].ILOffset)));
            return sb.ToString();
        }
    }
}

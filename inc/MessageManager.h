#pragma once
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <stack>

#include "bounded_queue.h"

namespace ntl
{

   enum class StatusResult : uint8_t
   {
      MSG_TIMEOUT,
      MSG_SUCCESS,
      MSG_FAILURE
   };

   enum class ReportedResult : uint8_t
   {
      RESULT_SUCCESS,
      RESULT_FAILURE
   };

   namespace detail
   {

      template <typename MsgTy, typename NodeIdTy = uint32_t>
      class MsgEntry
      {
      public:
         MsgEntry(uint32_t numResponsesNeeded, uint32_t numSuccessResponsesNeeded, const MsgTy& msg):
            m_NumResponsesNeeded(numResponsesNeeded),
            m_SuccessThreshold(numSuccessResponsesNeeded),
            m_NumAcknowledgementsReceived(0),
            m_NumFailuresReceived(0),
            m_Msg(msg)
         {
         }

         MsgEntry(uint32_t numResponsesNeeded, uint32_t numSuccessResponsesNeeded, MsgTy&& msg) :
            m_NumResponsesNeeded(numResponsesNeeded),
            m_SuccessThreshold(numSuccessResponsesNeeded),
            m_NumAcknowledgementsReceived(0),
            m_NumFailuresReceived(0),
            m_Msg(std::move(msg))
         {
         }

         MsgEntry(MsgEntry&& other) = default;

         void incrementNumAcknowledgementsReceived(const NodeIdTy& respondingNodeId)
         {
            std::unique_lock<decltype(m_WaitMtx)> lockGuard(m_WaitMtx);
            ++m_NumAcknowledgementsReceived;
            m_CondVar.notify_one();
         }

         void incrementNumFailuresReceived(const NodeIdTy& respondingNodeId)
         {
            std::unique_lock<decltype(m_WaitMtx)> lockGuard(m_WaitMtx);
            ++m_NumFailuresReceived;
            m_CondVar.notify_one();
         }

         MsgTy retrieveMessage() const
         {
            return m_Msg;
         }

         template <typename DurTy>
         StatusResult waitOnMessage(const std::chrono::duration<DurTy>& waitDuration)
         {
            std::unique_lock<decltype(m_WaitMtx)> lockGuard(m_WaitMtx);
            bool condSuccess = m_CondVar.wait_for(lockGuard, waitDuration, [&]() -> bool {
               return m_NumResponsesNeeded > (m_NumAcknowledgementsReceived + m_NumFailuresReceived);
            });

            StatusResult result = StatusResult::MSG_TIMEOUT;
            if (condSuccess)
            {
               if (m_NumAcknowledgementsReceived >= m_SuccessThreshold)
               {
                  result = StatusResult::MSG_SUCCESS;
               }
               else
               {
                  result = StatusResult::MSG_FAILURE;
               }
            }
         }


      private:
         const uint32_t m_NumResponsesNeeded;
         const uint32_t m_SuccessThreshold;
         uint32_t       m_NumAcknowledgementsReceived;
         uint32_t       m_NumFailuresReceived;
         MsgTy          m_Msg; 

         std::mutex              m_WaitMtx;
         std::condition_variable m_CondVar;
      };
   }

   template <typename MsgType, std::size_t MaxNumElems, typename KeyTy = uint32_t>
   class MessageManager
   {
   public:
      MessageManager()
      {
         for (std::size_t i = 0; i < MaxNumElems; ++i)
         {
            m_CtrlBlockQ.emplace(std::make_unique<detail::MsgEntry<MsgType>>(numResponsesNeeded, numSuccessResponsesNeeded, msg));
         }
      }

      bool registerMessage(const KeyTy& msgId, uint32_t numResponsesNeeded, const MsgType& msg)
      {
         return registerMessage(msgId, numResponsesNeeded, numResponsesNeeded, msg);
      }

      bool registerMessage(const KeyTy& msgId, uint32_t numResponsesNeeded, uint32_t numSuccessResponsesNeeded, const MsgType& msg)
      {
         auto entryPtr = std::make_unique<detail::MsgEntry<MsgType>>(numResponsesNeeded, numSuccessResponsesNeeded, msg);
         auto result = m_MsgTable.emplace(msgId, std::move(entryPtr));
         return result.second;
      }

      bool registerMessage(const KeyTy& msgId, uint32_t numResponsesNeeded, MsgType&& msg)
      {
         return registerMessage(msgId, numResponsesNeeded, numResponsesNeeded, std::move(msg));
      }

      bool registerMessage(const KeyTy& msgId, uint32_t numResponsesNeeded, uint32_t numSuccessResponsesNeeded, MsgType&& msg)
      {
         auto entryPtr = std::make_unique<detail::MsgEntry<MsgType>>(numResponsesNeeded, numSuccessResponsesNeeded, msg);
         auto result = m_MsgTable.emplace(msgId, std::move(entryPtr));
         return result.second;
      }

      bool tryRetrieveMessage(const KeyTy& msgId, MsgType& msg_out) const
      {
         bool result = false;
         auto findItr = m_MsgTable.find(msgId);
         if (findItr != m_MsgTable.end())
         {
            msg_out = findItr->second.retrieveMessage();
            result = true;
         }

         return result;
      }

      template <typename NodeIdTy>
      void updateMessageStatus(const NodeIdTy& nodeId, const KeyTy& msgId, ReportedResult reportedResult)
      {
         auto findItr = m_MsgTable.find(msgId);
         if (findItr != m_MsgTable.end())
         {
            switch (reportedResult)
            {
               case ReportedResult::RESULT_SUCCESS:
               {
                  findItr->second.incrementNumAcknowledgementsReceived(nodeId);
                  break;
               }

               case ReportedResult::RESULT_FAILURE:
               {
                  findItr->second.incrementNumFailuresReceived(nodeId);
                  break;
               }

               default:
               {
                  break;
               }
            }
         }
      }

      void removeMessage(const KeyTy& msgId)
      {
         auto findItr = m_MsgTable.find(msgId);
         if (findItr != m_MsgTable.end())
         {
            m_MsgTable.erase(findItr);
         }
      }

      template <typename DurTy>
      StatusResult waitOnMessage(const KeyTy& msgId, const std::chrono::duration<DurTy>& waitDuration)
      {
         StatusResult status = StatusResult::MSG_FAILURE;
         auto elem = m_MsgTable.find(msgId);

         if (elem != m_MsgTable.end())
         {
            status = elem->second->waitOnMessage(waitDuration);
         }

         return status;
      }

   private:
      std::unique_ptr <detail::MsgEntry<MsgType>> get_control_block_ptr()
      {
         std::unique_lock<decltype(m_QueueMtx)> lockGuard(m_QueueMtx);
         m_QueueCond.wait(lockGuard, [&]() {
            return !m_CtrlBlockQ.empty();
         });

         auto ret = std::move(m_CtrlBlockQ.front());
         m_CtrlBlockQ.pop();

         return ret;

      }

      std::mutex m_QueueMtx;
      std::condition_variable m_QueueCond;

      std::map<KeyTy, std::unique_ptr<detail::MsgEntry<MsgType>>> m_MsgTable;

      ntl::bounded_queue<std::unique_ptr<detail::MsgEntry<MsgType>>, MaxNumElems> m_CtrlBlockQ;
   };
}
